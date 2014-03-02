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
#include "host_input_control.h"
#include "host_hamac_pipe.h"

 
#if VERBOSE_INPUT_CONTROL >= 1
static t_uint32 buffct=0;
void display_list(char *s,t_bit_buffer *pin)
{
	t_uint16	i=0;
	t_bit_buffer *p=pin;

	while(p)
	{
		buffct++;
		p=(t_bit_buffer*)p->next;
		if (p==pin) NMF_PANIC("ERROR Reloop\n");
	}
	NMF_LOG("%s %d  ",s,buffct);

	p=pin;
	while(p) 
	{
		NMF_LOG("%p ",p);
		p=(t_bit_buffer*)p->next;
		if (p==pin) NMF_PANIC("ERROR Reloop\n");
	}
	NMF_LOG("\n");
}
void display_hw_list(char *s,t_bit_buffer *pin)
{
	t_uint16	i=0;
	t_bit_buffer *p=pin;

	while(p)
	{
		p=(t_bit_buffer*)p->next;
		if (p==pin) NMF_PANIC("ERROR Reloop\n");
	}
	NMF_LOG("%s %d  ",s,buffct);

	p=pin;
	while(p) 
	{
		NMF_LOG("%p ",p);
		p=(t_bit_buffer*)p->next;
		if (p==pin) NMF_PANIC("ERROR Reloop\n");
	}
	NMF_LOG("\n");
}

#endif


void METH(input_control_check_buffer_list)(t_bit_buffer *p_b)
{
	t_uint16	i=0;
	while (p_b) 
	{
		// Fix , for H264only (to be tested for other codec
		// problem found in FRAME mode , clearing the status leads to a double return of the buffer to proxy
		// Once when Hardware processing completes
		// Second when ARM completes (as inuse was previouusly clear by HW completion)
#ifndef __H264DEC__	
		// clear HW flag
		p_b->inuse &= ~VCL_UNDER_PROCESSING;
#endif
		if ((p_b->inuse &~VCL_UNDER_PROCESSING) ==0)  
		{

			#if VERBOSE_INPUT_CONTROL >= 2
		    NMF_LOG("Release input buffer %p\n",p_b);
			#endif
		}
		else
		{
			NMF_PANIC("ERROR can not Release input buffer because still use by SW) %p\n",p_b);
		}
		p_b=(t_bit_buffer*) p_b->next;
		i++;
	}

	#if  VERBOSE_INPUT_CONTROL >= 2
	NMF_LOG("Release nal count %d\n",i);
	#endif
}

#ifndef __H264DEC__
t_uint32 METH(pipe_is_ready)()
{
	if (( ! hamac_pipe_is_full()) && (input_control->dpb_frame_available >= 1) )
	{
		return 1;
	}
	return 0;
}
#elif __ndk5500_a0__
t_uint32 METH(pipe_is_ready)()
{
	if (( ! hamac_pipe_is_full()) && (input_control->dpb_frame_available >= 1) )
	{
		return 1;
	}
	return 0;
}

#endif


void METH(input_control_acknowledge)(t_bit_buffer *p_b, t_handshake mode) 
{
    // move hard coded XON to input parameter mode. input_control_acknowlege
    // function shall be called codec arm-nmf code in this way : 
    // input_control_acknowledge (..., XON);
	if (pipe_is_ready())
	{
		// free buffer only if not inuse
		if (p_b && (p_b->inuse==0))
		{
			// no link list because NON vcl
			p_b->next=0; 
			#if  VERBOSE_INPUT_CONTROL >= 1
			display_list("Input Buffer Release [ XON]  ",p_b);
			#endif
			nal_ctl.nal_handshake(mode,p_b);
		}
		else
		{
			#if  VERBOSE_INPUT_CONTROL >= 1
			display_list("Input Buffer Release [ XON]  ",0);
			#endif
			nal_ctl.nal_handshake(mode,0);
		}
	}
	else
	{
		// can not acknowledge so mark it as pending
		input_control->pending_ack++;

		// Inform proxy if buffer can be free
		if (p_b && (p_b->inuse==0))
		{
			#if VERBOSE_INPUT_CONTROL  >= 1
			display_list("Input Buffer Release [XOFF]  ",p_b);
			#endif
			nal_ctl.nal_handshake(XOFF,p_b);
		}
#if VERBOSE_INPUT_CONTROL  > 0
		else 
		{
			NMF_LOG("Input Buffer Release : pipe not ready , no buffer to release\n");
		}
#endif
	}
}


void METH(input_control_acknowledge_pending)(t_bit_buffer *p_b) 
{
	if (input_control->pending_ack && pipe_is_ready())
	{
		input_control->pending_ack--;	
		input_control_check_buffer_list(p_b);
		#if VERBOSE_INPUT_CONTROL >= 1
			display_list("Input Buffer Release [Pending XON]  ",p_b);
		#endif
		nal_ctl.nal_handshake(XON,p_b);
	}
	else
	{
		input_control_check_buffer_list(p_b);
		if (p_b)
		{
			
		#if VERBOSE_INPUT_CONTROL >= 1
			display_list("Input Buffer Release [Pending XOFF]  ",p_b);
		#endif
			
			nal_ctl.nal_handshake(XOFF,p_b);
		}
	}
}




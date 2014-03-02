/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * 
 * When a frame is bumped from the dpb, it may still be referenced.
 * - If it is NOT referenced, the frame it bumped out of the dpb 
 *   in an display queue which size depends on the latency of the display system.
 * - If it is referenced, the frame remains in the dpb.
 * In any case, the frame is marked as wait_tobe_free until 
 * the frame memory area is not needed by the display system anymore.
 *
 * The t_dec_buff structure can be seen as a generic dpb structure containing at least the following fields :
 * - pics_buf		: dpb array, containing dpb information
 * - DPBsize		: size of the DPB array, not including current decoded frame (=> +1 in the code)
 * - frames			: frames array, containing frames information (generic t_frames structure)
 * - max_nb_frames	: size of the frame array, reallocated if not big enouth
 * - ...			: other codec specific information
 *
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
#include "host_types.h"
#include "host_display_queue.h"
#include "local_alloc.h"

#if VERB_DPB == 1
t_uint16 frame_counter=0;
#endif

static inline void print_frames_info(t_dec_buff *p_buff) {
#if VERB_DPB == 2
	int i, j;
	NMF_LOG("frame\ttobefr\tavail\t");
	for (j=0; j < p_buff->DPBsize+1; j++) {
		NMF_LOG("%d\t",j);
	}
	NMF_LOG("\n");

	for (i=0; i<p_buff->max_nb_frames; i++) {
		NMF_LOG("%d\t%d\t%d\t", i, p_buff->frames[i].wait_tobe_free, p_buff->frames[i].is_available); 
		NMF_LOG("\n");
	}
#endif
}


// The current DPB location (dpb_loc) is no more referenced by the decoder
// if a frame is associated to this DPB location is should be release
void METH(display_queue_remove_unreferenced_frame)(t_dec_buff *p_buff, t_uint16 dpb_loc) 
{
	//A frame was associated to the DPB location
	if (p_buff->pics_buf[dpb_loc].frame_pos != -1) {

		t_frames *pframe = &p_buff->frames[p_buff->pics_buf[dpb_loc].frame_pos];
	   
		if (pframe->is_available == 0) { /* not available */
			if ((pframe->wait_tobe_free == 0) && 
					(! is_dpbused_frame(p_buff, pframe))) {
				pframe->is_available = 1;
				print_frames_info(p_buff);
				input_control_inc_dpb_frame_available();
				#if VERB_DPB == 1    
				NMF_LOG("frame %d unused, set available\n", dpb_loc);
				#endif
			}
			else {
				#if VERB_DPB == 1    
				NMF_LOG("Frame %d unused but waiting to be released, moved to display_queue\n", dpb_loc);
				#endif
			}
		}
		p_buff->pics_buf[dpb_loc].frame_pos = -1; 
	}
}


/* DPB needs an empty frame 														*/
/* search in the frame array if one frame is :										*/
/*			no more use by display "display_use==0" and								*/
/* 			not referenced by DPB													*/ 
t_sint16 METH(display_queue_get_frame_from_free_pool)(t_dec_buff *p_buff, t_uint32 size16) {
	int i=0;
	t_frames *pframe=0;
	print_frames_info(p_buff);
	while (i < p_buff->max_nb_frames)
	{
		pframe = &p_buff->frames[i];
		if ((pframe->is_available == 1) && (! is_dpbused_frame(p_buff, pframe)) && (pframe->ptr[0] != 0)) {
			pframe->wait_tobe_free = 0;
			pframe->nFlags = 0;
			print_frames_info(p_buff);
			input_control_dec_dpb_frame_available();

			pframe->ptr[1] = pframe->ptr[0] + size16;
			pframe->ptr[2] = pframe->ptr[1] + (size16>>2);
			return i;
		}
		i++;
	}
	return -1;
}


/// Call when a buffer is returned by the display system
// Search for the frame matching the ptr value , then decrement the associated "wait to be free" flag
//  because frame has been released by the display system
// If flag reach zero then frame is considered as no more used by display system
// if frame is also no more referenced by DPB , then frame is marked as available.
// If Search failed , then an empty free location in the frames array is used
// If no place available in the frames array, then we perform a realloc on it.
void METH(display_queue_new_output_buffer)(t_dec *dec, void *ptr) {
	int i;
	t_frames	*frame;
	//dpb_status(dec);

	for (i=0; i<dec->buf.max_nb_frames; i++) 
	{
		frame=&dec->buf.frames[i];
		if (frame->ptr[0] == ptr) 
		{
			frame->wait_tobe_free--;
        
			if (frame->wait_tobe_free < 0) {
                NMF_PANIC("file %s line %d: this case should not appear\n", __FILE__, __LINE__);
            }
			
			if ((frame->wait_tobe_free == 0) && (! is_dpbused_frame(&dec->buf, frame))) 
			{
				frame->is_available = 1;
				//print_frames_info(p_buff);
				input_control_inc_dpb_frame_available();
				input_control_acknowledge_pending(0);
			}
			#if VERB_DPB == 1    
		   	NMF_LOG("Get Back frame to fill %d\n", i);
			#endif
			return;
		}
	}

	// frame not found , its a new allocated frame by proxy , add it to an unused location
	frame=0;
	for (i=0; i<dec->buf.max_nb_frames; i++) 
	{
		if (dec->buf.frames[i].ptr[0] == 0)
		{
			frame=&dec->buf.frames[i];
            break;
		}
	}

	// no unused  location , so extend Frame array
	if (frame == 0) 
	{
		t_frames *old_frames = dec->buf.frames;
		dec->buf.max_nb_frames++;
		dec->buf.frames = (t_frames*)mallocCM(dec->buf.max_nb_frames * sizeof(t_frames), 0);
		if (dec->buf.frames==0)
		{
		 NMF_PANIC("Allocation failed dec buf frames\n");
		}
		memcpy(dec->buf.frames, old_frames, (dec->buf.max_nb_frames-1) * sizeof(t_frames));
		if (old_frames != 0) free(old_frames);
	    // last array location
		frame= &dec->buf.frames[dec->buf.max_nb_frames-1];
	}
    
	// add it to the frame array
	frame->ptr[0] = (t_img_pel)ptr;
	frame->wait_tobe_free = 0;
	frame->is_available = 1;
	//print_frames_info(p_buff);
	
	#if VERB_DPB == 1    
	NMF_LOG("Get New frame to fill\n");
	#endif
	input_control_inc_dpb_frame_available();
	input_control_acknowledge_pending(0);
}


#ifdef VERBOSE_OUTPUT_CHECKSUM
void output_cheksum(t_frameinfo *p_frameinfo, t_uint8 *p)
{   
	t_uint32	sum=0;
  	static t_uint32	picture_ct=0;
	t_uint32 size = ((p_frameinfo->common_frameinfo.pic_width)*(p_frameinfo->common_frameinfo.pic_height));
	NMF_LOG(" %d y%d\n",p_frameinfo->common_frameinfo.pic_width,p_frameinfo->common_frameinfo.pic_height);
	
	size+=size/2;
	picture_ct++;
	while (size--) sum+=*p++;
	NMF_LOG("Output checksum[%d] %08X  pointer %p\n",picture_ct,sum,p);
}
#endif


void METH(display_queue_output_picture)(t_frames *frame, t_picture_output picture_type, t_frameinfo *p_frameinfo) {
	#ifdef VERBOSE_OUTPUT_CHECKSUM
	if (p_frameinfo && (picture_type==VALID_PIC)) output_cheksum(p_frameinfo, (t_uint8*) frame->ptr[0]);
	#endif

	if (frame != 0) {
		output.picture(picture_type, frame->ptr[0], frame->nFlags, *p_frameinfo); 
	} else {
		output.picture(picture_type, 0, 0, *p_frameinfo); 
	}
}


void METH(display_queue_put)(t_dec_buff *p_buff, t_dpb_info *dpb_info, t_frames *frame) {
	/* add the bumped frame in display queue */
    if (! is_marked_frame(p_buff, dpb_info)) {
		/* bumped frame is no more referenced in the DPB */
		#if VERB_DPB == 1  
		NMF_LOG("Frame %d not referenced in DPB, stored in display queue\n", dpb_info->frame_pos);
		#endif
		dpb_info->frame_pos = -1;
	}
	else {
		/* bumped frame is referenced in the DPB */
		#if VERB_DPB == 1  
		NMF_LOG("Frame %d referenced in DPB\n", dpb_info->frame_pos);
		#endif
	}

	/* wait_tobe_free decremented when FiffThisBuffer provides again the same buffer (ptr comparaison)
	 * if wait_tobe_free==0 and the frame is not marked,
	 * => is_available = 1 */
    if (frame != 0) {
        if (frame->wait_tobe_free < 0) {
            NMF_PANIC("file %s line %d: wait_tobe_free < 0\n", __FILE__, __LINE__);
        }
        frame->wait_tobe_free++;
    }

	hamac_pipe_add_bumped_frame(frame);

	#if VERB_DPB == 1    
   	NMF_LOG("Output frame %d\n", frame_counter++);
	#endif
}

void METH(display_queue_reset_frames_array)(t_dec_buff *p_buff) {
    t_uint16 i;
	for (i=0; i<p_buff->max_nb_frames; i++) {
        p_buff->frames[i].ptr[0] = 0;
 		p_buff->frames[i].wait_tobe_free = 0;
 		p_buff->frames[i].is_available = 0;
 		p_buff->frames[i].nFlags = 0;
	}
}

t_uint16 METH(display_queue_init_frames_array)(t_dec_buff *p_buff, t_uint16 max_nb_frames, void *vfm_memory_ctxt) {
	p_buff->max_nb_frames = max_nb_frames;
	p_buff->frames = (t_frames*)mallocCM(p_buff->max_nb_frames * sizeof(t_frames), 0);
#ifdef __H264DEC__
	if (p_buff->frames ==0) return 1;
#else
	if (p_buff->frames ==0) NMF_PANIC("can not allocate  DPB buf frames");
#endif
	display_queue_reset_frames_array(p_buff);
	return 0;
}
void METH(display_queue_free_frames_array)(t_dec_buff *p_buff,void *vfm_memory_ctxt)
{
	free(p_buff->frames);
}	

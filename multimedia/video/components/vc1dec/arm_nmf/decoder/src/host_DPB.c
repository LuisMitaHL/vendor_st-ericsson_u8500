/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/**********************************************************
*   VC1 DPB is made of three buffers. 
*   Position 2 always old B pictures 
*            -> they are never used as ref and they are bumped immediately
*               so in practice they never really come into the DPB
*   Positions 0 and 1 hold the two reference pictures, they are smashed in
*     a circular way
*
*
*  Dec order     I0 P1 B2 B3 P4 B5 B6 P7 B8 B9 P10 B11   
*  Dpl order      - I0 B2 B3 P1 B5 B6 P4 B8 B9 P7  B11 P10
*
*   Between decoder and display there is always a delay of 1, so when we get to the end
*    of the stream we need to bump last ref 
***********************************************************/


#ifdef NMF_BUILD
#include <vc1dec/arm_nmf/decoder.nmf>
#endif

#include "host_types.h"
#include <vc1dec/arm_nmf/api/nmftype.idt>
#include "vc1d_common.h"
#include "host_DPB.h"
#include "host_display_queue.h"
#include "vc1d_co_reconstruction.h"
#include "nmf_lib.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_vc1dec_arm_nmf_proxynmf_decoder_src_host_DPBTraces.h"
#endif

/*****************************************************************************/
/**
 * \brief 	Initialize DPB buffer
 * \author 	Maurizio Colombo
 * \param   p_buff    pointer to DPB structure
 * \return  none
 */
/*****************************************************************************/
void InitBuffer(t_dec_buff *p_buff)
{
    t_uint16 i;
    
	  p_buff->DPBsize = 2; // current frame is not counted in DPB size
    
    for (i=0; i < p_buff->DPBsize+1; i++)
    {
		   p_buff->pics_buf[i].frame_pos = -1; //position in the frames array 
       p_buff->pics_buf[i].marked = 0;
       p_buff->pics_buf[i].need_display = 0;
       p_buff->pics_buf[i].ptr = (t_uint8**)NULL;
    }
    
    p_buff->oldest_ref = 0;
    p_buff->initialized = 1;
    
#if VERBOSE_STANDARD == 1
    NMF_LOG("\nDPB: initialized with size %i\n", p_buff->DPBsize);
#endif
}

/*****************************************************************************/
/**
 * \brief 	Insert a new frame in the DPB, in other words determine at which
 *           position of DPB we are going to decode current frame 
 *           Also compute reference frames for current frame 
 * \author 	Maurizio Colombo
 * \param   p_buff                          pointer to DPB structure
 * \param   picture_type_to_decode          type of current pic
 * \param   size                            size of Y buffer in bytes
 * \param   input_control                   handler to input control 
 * \return  none
 */
/*****************************************************************************/
t_sint16 METH(InsertFrame)(t_vc1_picture_type	picture_type_to_decode,
                           t_dec_buff *p_buff,  t_uint32 size,
                           t_sva_buffers * p_sva_buffers)
{
   t_sint16 index;
   
   // B and BI are not used as reference, so they are always allocated to pos 2 
   if((picture_type_to_decode==PICTURE_TYPE_B)||(picture_type_to_decode==PICTURE_TYPE_BI))
   {
       index = 2; 

       if(p_buff->pics_buf[!p_buff->oldest_ref].ptr!=NULL)
        p_sva_buffers->bwd_ref_frame = p_buff->pics_buf[!p_buff->oldest_ref].ptr[0];

       if(p_buff->pics_buf[p_buff->oldest_ref].ptr!=NULL)
        p_sva_buffers->fwd_ref_frame = p_buff->pics_buf[p_buff->oldest_ref].ptr[0];
       else
	 // seek mode : all buffers have been flushed from DPB
	 // and only a dummy reference have been inserted (grey pixels)
	 // -> can be used for forward + backward reference
	 p_sva_buffers->fwd_ref_frame = p_sva_buffers->bwd_ref_frame;
   }
   else
   {
       /* For I and P and Skip, idea is to smash oldest ref picture.
          When doing this, we mark that picture as unused for ref and 
          inform the display queue of this unmarking */
       index = p_buff->oldest_ref;
       p_buff->pics_buf[index].marked = 0;
       display_queue_remove_unreferenced_frame(p_buff, index);

       if(p_buff->pics_buf[!p_buff->oldest_ref].ptr!=NULL)
          p_sva_buffers->fwd_ref_frame = p_buff->pics_buf[!p_buff->oldest_ref].ptr[0];
       else
          p_sva_buffers->fwd_ref_frame = (t_uint8*)NULL;
                       
       p_sva_buffers->bwd_ref_frame = (t_uint8*)NULL;

       /* Then we swap oldest ref */
       if(p_buff->oldest_ref == 0)
         p_buff->oldest_ref = 1;
       else
         p_buff->oldest_ref = 0;
   }
   
     	OstTraceFiltInst1(TRACE_FLOW,  "VC1Dec: arm_nmf: DPB: InsertFrame(): position %d\n", index);
#if VERBOSE_STANDARD == 1   
   NMF_LOG("Insert Frame in DPB at position %d\n",index);
#endif
   
   // Now let's get the pointer to the frame
   /* if there is no pointer associated to current DPB position (frame_pos==-1) or if the
   pointer associated to current frame_pos has not got back yet from display, then get a 
   new buffer from the free pool */
	if ((p_buff->pics_buf[index].frame_pos == -1) || 
		(p_buff->frames[p_buff->pics_buf[index].frame_pos].is_available == 0)) {
		p_buff->pics_buf[index].frame_pos = display_queue_get_frame_from_free_pool(p_buff, size);
		p_buff->pics_buf[index].need_display = 1;
		p_buff->pics_buf[index].ptr = p_buff->frames[p_buff->pics_buf[index].frame_pos].ptr;
		p_buff->frames[p_buff->pics_buf[index].frame_pos].is_available = 0;
     	OstTraceFiltInst2(TRACE_FLOW,  "VC1Dec: arm_nmf: DPB: InsertFrame(): Got frame %d %x from free pool\n", p_buff->pics_buf[index].frame_pos, (unsigned int)p_buff->pics_buf[index].ptr[0]);
#if VERBOSE_STANDARD == 1
		NMF_LOG("Got frame %d %x from free pool\n",p_buff->pics_buf[index].frame_pos,p_buff->pics_buf[index].ptr[0]);
#endif
	}
   
   // Then, we mark the current picture as reference if it is not a B/BI
   if((picture_type_to_decode!=PICTURE_TYPE_B)&&(picture_type_to_decode!=PICTURE_TYPE_BI))
    p_buff->pics_buf[index].marked = 1;
   	
   return index;      
}

/*****************************************************************************/
/**
 * \brief 	Determine which picture will be output after current frame's decoding
 * \author 	Maurizio Colombo
 * \param   p_buff                          pointer to DPB structure
 * \param   picture_type_to_decode          type of current pic
 * \param   hamac_pipe                      handler to hamac pipe 
 * \return  none
 */
/*****************************************************************************/
t_sint16 METH(BumpFrame)(t_vc1_picture_type	picture_type_to_decode,
                         t_dec_buff *p_buff)
{
    t_sint16 pos=-1;
    
    /* if current picture is a B picture, then the current picture itself will be bumped */
    if((picture_type_to_decode==PICTURE_TYPE_B)||(picture_type_to_decode==PICTURE_TYPE_BI))
    {
        pos = 2;
    }
    else
    {   /* otherwise bump oldest ref */
        pos = p_buff->oldest_ref;
    }
    
    // if the picture to be bumped exists (this condition is false for first frame) and need to be display
    if(p_buff->pics_buf[pos].frame_pos!=-1 && p_buff->pics_buf[pos].need_display )
    { 
     	OstTraceFiltInst1(TRACE_FLOW,  "VC1Dec: arm_nmf: DPB: BumpFrame(): position %d\n", pos);
#if VERBOSE_STANDARD == 1    
       NMF_LOG("--> Bump frame %d\n",pos);
#endif
       display_queue_put(p_buff,&(p_buff->pics_buf[pos]),&(p_buff->frames[p_buff->pics_buf[pos].frame_pos]));
       p_buff->pics_buf[pos].need_display = 0;
    }
    return pos;
}


/*****************************************************************************/
/**
 * \brief 	Flush the DPB
 * \author 	Olivier Barault
 * \param   	p_buff       		pointer to DPB structure
 * \return  	none
 */
/*****************************************************************************/
void METH(FlushDPB)(t_dec_buff *p_buff)
{
    	t_uint16 i;

    	/* Mark all frames as unused for reference */
    	for (i=0; i < p_buff->DPBsize+1; i++)
    	{
        	p_buff->pics_buf[i].marked = 0;
    	}
 
    	// Output frame remaining in DPB (most recent ref still not display)
	// BumpFrame will choose oldest ref, so let's mark most recent ref as oldest ref
      	if(p_buff->oldest_ref == 0)
        	p_buff->oldest_ref = 1;
      	else
        	p_buff->oldest_ref = 0;
      	// Here call it with Pic type != B in order to make it bump the oldest ref
      	BumpFrame(PICTURE_TYPE_I,p_buff);    
      			
	/* free frames */
	if (DISPLAY_QUEUE_SIZE != 0) {
		for (i=0; i < p_buff->DPBsize+1; i++) {			
			display_queue_remove_unreferenced_frame(p_buff, i);
		}
	}   
}


/*****************************************************************************/
/**
 * \brief 	Insert dummy reference frame in DPB (used after seek)
 * \author 	Olivier Barault
 * \param   	p_buff       		pointer to DPB structure
 * \param   	pic_width               reference frame width
 * \param   	pic_height              reference frame height
 * \return  	none
 */
/*****************************************************************************/
void METH(InsertDummyRefIntoDpb)(t_dec_buff *p_buff,
			   t_uint16 pic_width, 
			   t_uint16 pic_height)
{
  t_sva_buffers sva_buffers;
  t_sint16 index;

  ts_picture_buffer picture;
  ts_rec_param rec_param;
  t_uint32 buffer_size;
 
  // add 1 reference frame in DPB
  index = InsertFrame(PICTURE_TYPE_I,p_buff, 0, &sva_buffers); 		    
  p_buff->frames[p_buff->pics_buf[index].frame_pos].nFlags = OSI_OMX_BUFFERFLAG_DECODEONLY;
  
  // size of buffer to fill
  picture.y = p_buff->pics_buf[index].ptr[0];
  arm_rec_evaluate_rec_param_from_picture_dimension(pic_width,pic_height,&rec_param);
  arm_rec_initialize_destination_pointers(&picture,&rec_param);
  buffer_size = picture.y_size + picture.cb_size + picture.cr_size;
  
  // fill full buffer with grey pixels
  memset((t_uint8*)(p_buff->frames[p_buff->pics_buf[index].frame_pos].ptr[0]),0x80,buffer_size);
      
  return;
}


t_sint16 is_marked_frame(t_dec_buff *p_buff, t_dpb_info *dpb_info) 
{
	if (dpb_info->marked == 1) return 1;
	return 0;
}

/**
 * \brief search if frame is marked as referenced, or need to be displayed, in the DPB
 * for this purpose all DPB location are tested to see if the frame used by the
 * current DPB location match the searched one
 * if yes, then it check if DPB location is still used (marked or need_display)
 */
t_sint16 is_dpbused_frame(t_dec_buff *p_buff, t_frames *frame) {
	int j;
	// scanning all frame in DPB
	for (j=0; j < p_buff->DPBsize+1; j++) 
	{
		t_dec_pic_info	*pic_info=&p_buff->pics_buf[j];
		
		// DPB location use the same frame as the search one
		if	( ( pic_info->frame_pos>=0) && (frame == &p_buff->frames[pic_info->frame_pos] ))
		{
			if (pic_info->marked == 1)  return 1;
		}
	}
	return 0;
}

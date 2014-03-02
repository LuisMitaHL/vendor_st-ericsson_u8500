/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef NMF_BUILD
#include <h264dec/arm_nmf/decoder.nmf>
#endif

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_arm_nmf_decoder_src_host_DPBTraces.h"
#endif


#ifndef __SYMBIAN32__
#include <los/api/los_api.h>
#endif

#include "types.h"

#include "host_types.h"
#include "macros.h"
#include "settings.h"
#include "host_DPB.h"
#include "host_display_queue.h"
#include "host_frameinfo.h"
#include "host_block_info.h"


/* Local function prototypes */
static t_sint32 DecodePOC(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff);
static	t_sint16 FindLong(t_uint16 DPBsize, t_uint16 long_id, t_dec_pic_info *p_pics_buf);
static  t_sint16 FindShort(t_uint16 DPBsize, t_sint32 short_id, t_dec_pic_info *p_pics_buf);
t_uint16 ConcealFrame(t_uint16 prevNum, t_uint16 pos, t_seq_par *sp, t_dec_buff *p_buff);

#if VERB_DPB == 1
void DumpDPB(t_dec_buff *p_buff);
#endif

/**
 * Special function to patch timestamps from demuxer if they are either not provided or if they are wrong
 * (case of streams with B frames but no CTTS atoms for instance)
 **/
static void h264dec_TimestampsManagement(t_dec_buff *p_buff, t_uint16 pos, t_dec * dec)
{
  OMX_S64 loc_timestamp;

  /* demuxer has provided at least one non-null timestamp, disable auto-generation test mode */
  if((p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampH != 0)
   ||(p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampL != 0))
            dec->g_no_timestamps_from_demuxer = 0;

  /**************************************************************
   *  here is the case when timestamps are not given to decoder
   *  In principle this should just be a test mode
   *  Note that for first frame we just override zero with zero
   **************************************************************/
   if((dec->g_no_timestamps_from_demuxer == 1)
    &&(dec->p_active_sp->vui.timing_info_present_flag == 1)
    &&(dec->p_active_sp->vui.fixed_frame_rate_flag == 1)
    &&(p_buff->g_decoded_frm_ctr != 0))
      {
          OstTraceInt0( TRACE_WARNING , "H264DEC: Demuxer provides no timestamps, auto-generating them\n");
#if VERBOSE_STANDARD == 1
          NMF_LOG("H264DEC: Demuxer provides no timestamps, auto-generating them\n");
#endif
          loc_timestamp = (OMX_S64)((OMX_S64)(dec->p_active_sp->vui.num_units_in_tick) * (OMX_S64)2000000);
          loc_timestamp *= (OMX_S64)(p_buff->g_decoded_frm_ctr);
          loc_timestamp /= (OMX_S64)(dec->p_active_sp->vui.time_scale);
          p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampH
                        = (loc_timestamp>>32) & 0xFFFFFFFF;
          p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampL
                        = (loc_timestamp    ) & 0xFFFFFFFF;
          //NMF_LOG("TIMESTAMP %lli\n",loc_timestamp);
      }

    // check if timestamps from demuxer are wrong
    // Rule is that at decoder output timestamps must be monotonically increasing
    if(p_buff->g_decoded_frm_ctr!=0)
    {
     loc_timestamp = (((OMX_S64)p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampH) << 32)
                   | (((OMX_S64)p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampL) & 0xFFFFFFFF);
     if((loc_timestamp < dec->g_last_timestamp)&&(dec->g_wrong_timestamps==0))
     {
       OstTraceInt0( TRACE_WARNING , "H264DEC: Timestamps provided by demuxer are wrong\n");
       dec->g_wrong_timestamps = 1;
     }
    }

    // Timestamps from demuxer are wrong, let's generate them
    if(dec->g_wrong_timestamps)
    {
        // if framerate has been set at OMX level, use it to auto-generate TS
        // This info is extracted from MP4/3GP container
        if(dec->g_omx_framerate!=0)
        {
            // compute as (2^16 / fr) * 10^6 * counter
            // g_omx_framerate is expressed in Q15.16 format (fr*2^-16)
            loc_timestamp = (OMX_S64)(65536000000ULL);
            loc_timestamp *= (OMX_S64)(p_buff->g_decoded_frm_ctr);
            loc_timestamp /= (OMX_S64)(dec->g_omx_framerate);
            loc_timestamp += (dec->g_start_time);
            p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampH
                          = (loc_timestamp>>32) & 0xFFFFFFFF;
            p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampL
                          = (loc_timestamp    ) & 0xFFFFFFFF;
        }
        else if((dec->p_active_sp->vui.timing_info_present_flag == 1)
                &&(dec->p_active_sp->vui.fixed_frame_rate_flag == 1)
                &&(dec->p_active_sp->vui.time_scale != 0))
        {
          // use VUI informations to generate timestamps
          loc_timestamp = (OMX_S64)((OMX_S64)(dec->p_active_sp->vui.num_units_in_tick) * (OMX_S64)2000000);
          loc_timestamp *= (OMX_S64)(p_buff->g_decoded_frm_ctr);
          loc_timestamp /= (OMX_S64)(dec->p_active_sp->vui.time_scale);
          loc_timestamp += (dec->g_start_time);
          p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampH
                        = (loc_timestamp>>32) & 0xFFFFFFFF;
          p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampL
                        = (loc_timestamp    ) & 0xFFFFFFFF;
        }
        else
        {
          OstTraceInt0( TRACE_WARNING , "H264DEC: No information available to generate timestamps !!\n");
#if VERBOSE_STANDARD == 1
          NMF_LOG("H264DEC: No information available to generate timestamps !!\n");
#endif
        }

    } // wrong timestamps

    dec->g_last_timestamp = (((OMX_S64)p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampH) << 32)
                                 | (((OMX_S64)p_buff->frames[p_buff->pics_buf[pos].frame_pos].frameinfo.common_frameinfo.nTimeStampL) & 0xFFFFFFFF);
    p_buff->g_decoded_frm_ctr++;
}


/**
 * \brief Initialize Decoded Picture Buffer
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function initialize Decoded Picture Buffer.
 */

void METH(InitBuffer)(t_dec_buff *p_buff)
{
    t_uint16 i;

    for (i=0; i < MAXNUMFRM; i++)
    {
		p_buff->pics_buf[i].frame_pos = -1; //FIXME JMG moved from 0 to -1
		p_buff->pics_buf[i].ptr = (t_img_pel *)NULL; // added by MC
		p_buff->pics_buf[i].cup_ctx_idx = -1;
        p_buff->pics_buf[i].marked_short = 0;
        p_buff->pics_buf[i].marked_long = 0;
        p_buff->pics_buf[i].need_display = 0;
        p_buff->pics_buf[i].pic_num = MIN_SINT_32;
    }

    p_buff->DPBfullness = 0;
    p_buff->MaxLongTermFrameIdx = -1;
    p_buff->numShortRef = 0;
    p_buff->numLongRef = 0;
    p_buff->initialized = 1;

    OstTraceFiltInst1( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: DPB: initialized with size %i\n", p_buff->DPBsize);
#if VERB_DPB == 1
    NMF_LOG("\nDPB: initialized with size %i\n", p_buff->DPBsize);
#endif
}

/***********************************************************************
 * This function gives back to ddep, as invalid pics,
 * the output buffers being used in the DPB,
 * reverting the DPB back to an initialized state.
 * This is needed when changing dynamically port settings
 ***********************************************************************/
void METH(DPB_Revert)(t_dec_buff *p_buff)
{
  t_uint16 i;
  OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: DPB Revert\n");
  for (i=0; i < MAXNUMFRM; i++)
  {
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    // Give back to CUP FIFO the CUP context buffer which is in use
   	if(cup_context_fifo_push(&p_buff->cup_ctx_desc,p_buff->pics_buf[i].cup_ctx_idx)==-1)
   	{
        OstTraceInt0( TRACE_ERROR,"H264DEC: CUP context push failed: fifo full\n");
    		NMF_PANIC("CUP context push failed: fifo full\n");
		}
#endif
    if(p_buff->pics_buf[i].frame_pos != -1)
    {
		  t_frames * frame = &p_buff->frames[p_buff->pics_buf[i].frame_pos];
		  frame->is_available = 0;
   		set_frameinfo(&(frame->frameinfo), dec_static.p_active_sp, &dec_static.sh[0], 0, 0, 0);
   		display_queue_output_picture(frame, INVALID_PIC, &(frame->frameinfo));
   		p_buff->pics_buf[i].frame_pos = -1;
	   }
  }
  InitBuffer(p_buff);
}

/*******************************************************
 * Insert dummy frame for seek at the beginning of DPB
 *******************************************************/
void METH(InsertDummyRefIntoDPB)(t_dec_buff *p_buff, t_uint16 prevNum, t_seq_par *p_sp)
{
    t_uint16 mbx = (p_sp->pic_width_in_mbs_minus1+1);
    t_uint16 mby = ((p_sp->pic_height_in_map_units_minus1+1)
                    *(2 - p_sp->frame_mbs_only_flag));
    t_uint32 size = (mbx * mby) << 7;
    t_uint32 dummy = 0;

    OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: InsertDummyRefIntoDPB\n");

    /* Initialize frame infos */
    p_buff->pics_buf[0].frame_num = prevNum;
    p_buff->pics_buf[0].frame_num_wrap = p_buff->pics_buf[0].pic_num = p_buff->pics_buf[0].frame_num;
    p_buff->pics_buf[0].poc = MIN_SINT_32;
    p_buff->pics_buf[0].marked_short = 1;
    p_buff->pics_buf[0].marked_long = 0;
    p_buff->pics_buf[0].need_display = 1;
    // in case of dummy buffer, the buffer will be bumped but with a decode_only flag,
    // so that DDEP will send it back directly to decoder without sending it to application
    // Sending it to app would be an issue from timestamp point of view

    p_buff->numShortRef++;
    p_buff->DPBfullness++;

    p_buff->pics_buf[0].frame_pos = display_queue_get_frame_from_free_pool(p_buff, size);
    p_buff->pics_buf[0].ptr = p_buff->frames[p_buff->pics_buf[0].frame_pos].ptr;
    p_buff->frames[p_buff->pics_buf[0].frame_pos].nFlags |= OSI_OMX_BUFFERFLAG_DECODEONLY;
    p_buff->frames[p_buff->pics_buf[0].frame_pos].is_available = 0;

    dec_static.tmp_sh.disable_deblocking_filter_idc = 0; // just to have a dummy value
    set_TimeStamp(&(p_buff->pics_buf[0]), &dummy, &dummy); // dummy timestamp
	  set_frameinfo(&(p_buff->frames[p_buff->pics_buf[0].frame_pos].frameinfo), p_sp, &dec_static.tmp_sh, 0, 0, 0);

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    if(cup_context_fifo_push(&p_buff->cup_ctx_desc,p_buff->pics_buf[0].cup_ctx_idx)==-1)
    {
        OstTraceInt0( TRACE_ERROR,"H264DEC: CUP context push failed: fifo full\n");
		      	  NMF_PANIC("CUP context push failed: fifo full\n");
    }
    p_buff->pics_buf[0].cup_ctx_idx = cup_context_fifo_pop(&p_buff->cup_ctx_desc);
    if(p_buff->pics_buf[0].cup_ctx_idx == -1)
    {
     OstTraceInt0( TRACE_ERROR,"H264DEC: CUP context FIFO empty\n");
     NMF_PANIC("CUP context FIFO empty\n");
    }
#endif

    // Grey image
    memset(p_buff->frames[p_buff->pics_buf[0].frame_pos].ptr[0],0x80, size*sizeof(t_uint16));
    memset(p_buff->frames[p_buff->pics_buf[0].frame_pos].ptr[1],0x80,(size>>2)*sizeof(t_uint16));
    memset(p_buff->frames[p_buff->pics_buf[0].frame_pos].ptr[2],0x80,(size>>2)*sizeof(t_uint16));
}


/**
 * \brief Perform bumping process on DPB
 * \param p_sp Pointer to the active sequence parameter set structure
 * \param p_buff Pointer to buffers structure
 * \return 1 if a frame was output, 0 otherwise
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function performs bumping process on DPB.
 */

t_sint16 METH(BumpFrame)(t_dec_buff *p_buff)
{
    t_sint32 poc;
    t_uint16 i;
    t_sint16 pos;

    /* Find smallest POC in the DPB */
    poc = MAX_SINT_32;
    pos = -1;

    for (i=0; i < p_buff->DPBsize+1; i++)
    {
        if ((p_buff->pics_buf[i].need_display) && (poc > p_buff->pics_buf[i].poc))
        {
            poc = p_buff->pics_buf[i].poc;
            pos = i;
        }
    }

    if (pos < 0)
        return -1;

    OstTraceFiltInst3(H264DEC_TRACE_GROUP_VERB_DPB," H264DEC: Frame at position %d in dpb and %d in frames with POC %li sent to display_queue\n", pos,p_buff->pics_buf[pos].frame_pos,(t_uint32)(poc&0xFFFFFFFFul));
#if VERB_DPB == 1
    NMF_LOG("\nFrame at position %d in dpb and %d in frames with POC %li sent to display_queue\n",
			pos,
			p_buff->pics_buf[pos].frame_pos,
			poc);
#endif

  h264dec_TimestampsManagement(p_buff, pos, &dec_static);

	display_queue_put(p_buff, &(p_buff->pics_buf[pos]), &p_buff->frames[p_buff->pics_buf[pos].frame_pos]);
	p_buff->pics_buf[pos].need_display = 0;

    if ((!p_buff->pics_buf[pos].marked_short) && (!p_buff->pics_buf[pos].marked_long))
    {
        p_buff->DPBfullness--;
    }

    return pos;
}



/**
 * \brief Check DPB for unused frame buffers
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function adjust the DPB fullness if unused frame buffers are found.
 */

t_uint16 METH(RemoveUnusedFrame)(t_dec_buff *p_buff)
{
    t_uint16 i;

    /* Remove frames that were already output and are no longer marked as used for reference */
    for (i=0; i < p_buff->DPBsize+1; i++)
    {
        if ((!p_buff->pics_buf[i].need_display) &&
			(!p_buff->pics_buf[i].marked_short) && (!p_buff->pics_buf[i].marked_long))
        {
            p_buff->DPBfullness--;
		      	display_queue_remove_unreferenced_frame(p_buff, i);
            return 1;
        }
    }
    return 0;
}



/**
 * \brief Flush the DPB
 * \param p_sp Pointer to the active sequence parameter set structure
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function flush the DPB by sending to output all frames that need display in POC order.
 */

void METH(FlushDPB)(t_dec_buff *p_buff, t_sint16 keep_frame)
{
    t_uint16 i;

    OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: FlushDPB\n");

    /* Mark all frames as unused for reference */
    for (i=0; i < p_buff->DPBsize+1; i++)
    {
        p_buff->pics_buf[i].marked_short = 0;
        p_buff->pics_buf[i].marked_long = 0;
    }

    p_buff->numShortRef = 0;
    p_buff->numLongRef = 0;
    p_buff->MaxLongTermFrameIdx = -1;

    /* Output frames in POC order */
	while (BumpFrame(p_buff) != -1)
	{
	};

	/* free frames */
	if (DISPLAY_QUEUE_SIZE != 0) {
		for (i=0; i < p_buff->DPBsize+1; i++) {
			if (p_buff->pics_buf[i].frame_pos != keep_frame) {
				display_queue_remove_unreferenced_frame(p_buff, i);
			}
		}
	}

    p_buff->DPBfullness = 0;
}



/**
 * \brief Specific DPB IDR management
 * \param p_sp Pointer to the active sequence parameter set structure
 * \param p_sh Pointer to the slice header structure
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function performs all operations needed when a new IDR frame is being decoded.
 */
#if CLEAN_BUFFER == 1
void	ClearPicture(t_dec_buff *p_buff,t_seq_par *p_sp)
{

		t_uint32 size = ((p_sp->pic_width_in_mbs_minus1+1)*(p_sp->pic_height_in_map_units_minus1+1)) << 8;
        (void)memset((void*)p_buff->pics_buf[0].ptr[0], 0, (size_t)(size));
        (void)memset(p_buff->pics_buf[0].ptr[1], 0, (size_t)(size/4));
        (void)memset(p_buff->pics_buf[0].ptr[2], 0, (size_t)(size/4));
}
#endif

void METH(NewIDR)(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff,
                  t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags,
                  t_SEI *sei)
{
    t_dec_pic_info *p_dpi;
    t_uint16 mbx, mby;
	t_uint32 size16;

    if (p_sh->no_output_of_prior_pics_flag)
    {
       if(p_buff->initialized==1) // make sure that any output buffer in use is given back
       {
              t_uint16 i;
              for (i=0; i < MAXNUMFRM; i++)
              {
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
                // Give back to CUP FIFO the CUP context buffer which is in use
               	if(cup_context_fifo_push(&p_buff->cup_ctx_desc,p_buff->pics_buf[i].cup_ctx_idx)==-1)
               	{
               	        OstTraceInt0( TRACE_ERROR,"H264DEC: CUP context push failed: fifo full\n");
            		    NMF_PANIC("CUP context push failed: fifo full\n");
            		}
#endif
                if(p_buff->pics_buf[i].frame_pos != -1)
                {
            		  t_frames * frame = &p_buff->frames[p_buff->pics_buf[i].frame_pos];
            		  if(frame->wait_tobe_free == 0)
            		  {
            		    frame->is_available = 1;
            				input_control_inc_dpb_frame_available();
            			}
                }
              }
       }
       InitBuffer(p_buff);
    }
    else
    {
        FlushDPB(p_buff, -1);
    }

    /* Update informations for POC decoding */
    p_buff->FrameNumOffset = 0;
    p_buff->PrevPicOrderCntMsb = 0;
    p_buff->PrevPicOrderCntLsb = 0;

    p_dpi = &p_buff->pics_buf[0];

	mbx = (p_sp->pic_width_in_mbs_minus1+1);
    mby = (p_sp->pic_height_in_map_units_minus1+1) * (2 - p_sp->frame_mbs_only_flag);
	size16 = ((mbx * mby) << 8)/2;
	p_buff->pics_buf[0].frame_pos = display_queue_get_frame_from_free_pool(p_buff, size16);
	if (p_buff->pics_buf[0].frame_pos == -1) {
    OstTraceInt0( TRACE_ERROR,"H264DEC: error: no new frame available\n");
		NMF_PANIC("\nERROR: no new frame available.\n");
	}
	p_buff->pics_buf[0].ptr = p_buff->frames[p_buff->pics_buf[0].frame_pos].ptr;
	p_buff->frames[p_buff->pics_buf[0].frame_pos].is_available = 0;
    p_buff->frames[p_buff->pics_buf[0].frame_pos].nFlags = nFlags;

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
  if(cup_context_fifo_push(&p_buff->cup_ctx_desc,p_buff->pics_buf[0].cup_ctx_idx)==-1)
  {
       OstTraceInt0( TRACE_ERROR,"H264DEC: CUP context push failed: fifo full\n");
    	  NMF_PANIC("CUP context push failed: fifo full\n");
	}
  p_buff->pics_buf[0].cup_ctx_idx = cup_context_fifo_pop(&p_buff->cup_ctx_desc);
  if(p_buff->pics_buf[0].cup_ctx_idx == -1)
  {
    OstTraceInt0( TRACE_ERROR,"H264DEC: CUP context FIFO empty\n");
    NMF_PANIC("CUP context FIFO empty\n");
  }
#endif

    /* Initialize frame infos */
    p_dpi->frame_num = p_sh->frame_num;
    p_dpi->poc = DecodePOC(p_sp, p_sh, p_buff);
    p_dpi->marked_short = 0;
    p_dpi->pic_num = 0;
    p_dpi->marked_long = 0;
    p_dpi->need_display = 1;

#if CLEAN_BUFFER == 1
	ClearPicture(p_buff,p_sp);
#endif
    /* no needs related to output fifo because FlushPB */
	p_buff->DPBfullness++;

    /* Set pointers to current frame and frame info */
    p_buff->curr_frame = p_dpi->ptr;
    p_buff->curr_info = p_dpi;

#if ENABLE_TIMING == 1
    if (DecodeTime(sei))
    {
      p_buff->curr_info->display_time = sei->timings.t_o_dpb;
    }
#endif

	set_TimeStamp(p_buff->curr_info, &nTimeStampH, &nTimeStampL);
	set_frameinfo(&(p_buff->frames[p_buff->pics_buf[0].frame_pos].frameinfo), p_sp, p_sh,
                  nTimeStampH, nTimeStampL, p_buff->curr_3d_format);
	hamac_pipe_set_frame_pos(p_buff->pics_buf[0].frame_pos);

    OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: DPB: new IDR frame at position 0\n");
#if VERB_DPB == 1
    NMF_LOG("\nDPB: new IDR frame at position 0\n");
    DumpDPB(p_buff);
#endif
}



/**
 * \brief Initialize DPB for a new frame
 * \param p_sp Pointer to the structure with active sequence parameter set
 * \param p_sh Pointer to the structure with current slice header information
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>Modified interface.</td></tr>
 * </table>
 * </dl>
 *
 * The function initialize DPB for a new frame. Decoding of gaps in frame number is also performed.
 */

t_uint16 METH(NewFrame)(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff,
                        t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags,
                        t_SEI* sei)
{
    t_sint16 index;
    t_dec_pic_info *p_dpi;
    t_uint32 MaxFrameNum = (1 << (p_sp->log2_max_frame_num_minus4 + 4));
    t_uint16 prevNum = (p_buff->PreviousFrameNum + 1) % MaxFrameNum;

    /* Check for gaps in frame number */
    /* This handles in the same way the frame loss and the non-existing frames ! */
    if ((p_sh->frame_num != p_buff->PreviousFrameNum) && (p_sh->frame_num != prevNum))
    {
      do
      {
          index = InsertFrame(prevNum, MaxFrameNum, p_sp, p_buff);

          /* Conceal missing frame */
          if (ConcealFrame(prevNum, index, p_sp, p_buff))
          {
              //NMF_LOG("\nUnable to conceal missing frame %i\n", prevNum);
              return 2;
          }
	        else
          {
		           input_control_inc_dpb_frame_available();
	        }

          //NMF_LOG("\nMissing or non-existing frame %i. Concealed.\n", prevNum);
          prevNum = (prevNum + 1) % MaxFrameNum;
      }
      while (prevNum != p_sh->frame_num);
    }

    if ((FindShort(p_buff->DPBsize, p_sh->frame_num, p_buff->pics_buf) != -1) && (p_sh->nri > 0))
    {
        return 1;
    }

    index = InsertFrame(p_sh->frame_num, MaxFrameNum, p_sp, p_buff);

    p_dpi = &p_buff->pics_buf[index];

    /* Initialize frame infos */
    p_dpi->frame_num = p_sh->frame_num;
    p_dpi->frame_num_wrap = p_dpi->pic_num = p_dpi->frame_num;
    p_dpi->poc = DecodePOC(p_sp, p_sh, p_buff);
    p_dpi->marked_short = 0;
    p_dpi->marked_long = 0;
    p_dpi->need_display = 1;

	p_buff->frames[p_buff->pics_buf[index].frame_pos].is_available = 0;
	p_buff->frames[p_buff->pics_buf[index].frame_pos].nFlags = nFlags;

    p_buff->DPBfullness++;

    /* Set pointers to current frame and frame info */
    p_buff->curr_frame = p_dpi->ptr;
    p_buff->curr_info = p_dpi;

#if ENABLE_TIMING == 1
    if (DecodeTime(sei))
    {
      p_buff->curr_info->display_time = sei->timings.t_o_dpb;
    }
#endif

	set_TimeStamp(p_buff->curr_info, &nTimeStampH, &nTimeStampL);
	set_frameinfo(&(p_buff->frames[p_buff->pics_buf[index].frame_pos].frameinfo), p_sp, p_sh,
                nTimeStampH, nTimeStampL, p_buff->curr_3d_format);
	hamac_pipe_set_frame_pos(p_buff->pics_buf[index].frame_pos);

    OstTraceFiltInst2( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: DPB: new frame at position %d in dpb and %d in frames\n",index, p_buff->pics_buf[index].frame_pos);
#if VERB_DPB == 1
    NMF_LOG("\nDPB: new frame at position %d in dpb and %d in frames\n",
			index, p_buff->pics_buf[index].frame_pos);
    DumpDPB(p_buff);
#endif

    return 0;
}



/**
 * \brief Insert a frame in the DPB
 * \param frame_num Frame number
 * \param MaxFrameNum Maximum frame number obtained by sequence parameter set
 * \param p_sp Pointer to the active sequence parameter set structure
 * \param p_buff Pointer to buffers structure
 * \return Index of the selected position into the DPB
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function insert a frame in the DPB using sliding window memory control.
 */

t_sint16 METH(InsertFrame)(t_uint16 frame_num, t_uint32 MaxFrameNum, t_seq_par *p_sp, t_dec_buff *p_buff)
{
    t_uint16 i = 0;
    t_sint16 index;
    t_uint16 mbx, mby;
	t_uint32 size16;

    /* Update frame_num_wrap if necessary */
    for(i=0; i < p_buff->DPBsize+1; i++)
    {
        if (p_buff->pics_buf[i].marked_short)
        {
            if (p_buff->pics_buf[i].frame_num > frame_num)
                p_buff->pics_buf[i].frame_num_wrap = p_buff->pics_buf[i].frame_num - MaxFrameNum;
            else
                p_buff->pics_buf[i].frame_num_wrap = p_buff->pics_buf[i].frame_num;

            p_buff->pics_buf[i].pic_num = p_buff->pics_buf[i].frame_num_wrap;
        }
    }

    index = -1;

    if ((p_buff->numLongRef + p_buff->numShortRef) == (p_sp->num_ref_frames+1)) /* Sliding window */
    {
        /* Find smaller short ref id and mark as unused for short ref*/
        for (i = 0; i < p_buff->DPBsize+1; i++)
        {
            if (p_buff->pics_buf[i].marked_short)
            {
                if (index == -1)
                {
                    index = i;
                }
                else if (p_buff->pics_buf[i].pic_num < p_buff->pics_buf[index].pic_num)
                {
                    index = i;
                }
            }
        }

          /* check index value to be more robust */
          if(index!=-1)
          {
              p_buff->pics_buf[index].marked_short = 0;
              p_buff->numShortRef--;
		          remove_unmarked_displayed_frames(p_buff, index);
		      }
    }

    /* Check if DPB is full and try to remove unused frame */
    if (p_buff->DPBfullness == (p_buff->DPBsize+1))
    {
        RemoveUnusedFrame(p_buff);
    }

    /* Check if DPB is still full and apply bumping process */
    while (p_buff->DPBfullness == (p_buff->DPBsize+1))
    {
        if (BumpFrame(p_buff) == -1)
            break;
    }

    /* If at this point the DPB is still full there must be an error */
    if (p_buff->DPBfullness == (p_buff->DPBsize+1))
    {
        OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: DPB full. No room for new frame. Dropping oldest frame.\n");
#if VERB_ERR_CONC == 1
        NMF_LOG("\nWARNING: DPB full. No room for new frame. Dropping oldest frame.\n");
#endif
        /* Try to perform again RemoveUnusedFrame */
        RemoveUnusedFrame(p_buff);

        if (p_buff->DPBfullness == (p_buff->DPBsize+1))
        {
            /* Remove frame with lowest frame number */
            index = -1;

            for (i = 0; i < p_buff->DPBsize+1; i++)
            {
                if (p_buff->pics_buf[i].marked_short)
                {
                    if (index == -1)
                    {
                        index = i;
                    }
                    else if (p_buff->pics_buf[i].pic_num < p_buff->pics_buf[index].pic_num)
                    {
                        index = i;
                    }
                }
            }

          /* check index value to be more robust */
          if(index!=-1)
          {
            p_buff->pics_buf[index].marked_short = 0;
            p_buff->numShortRef--;
			       remove_unmarked_displayed_frames(p_buff, index);
			    }
        }

        if (p_buff->DPBfullness == (p_buff->DPBsize+1))
            FlushDPB(p_buff, -1);
    }

    /* Find a free position in the DPB */
    index = -1;
    i = 0;
    do
    {
        if (!p_buff->pics_buf[i].need_display && !p_buff->pics_buf[i].marked_short && !p_buff->pics_buf[i].marked_long)
            index = i;

        i++;

    } while ((i < p_buff->DPBsize+1) && (index == -1));

    /* Check for errors */
    if (index == -1)
    {
        /* If we are here there are some frames that are not marked as used for reference but need display */
        OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: DPB WARNING: Forced output of a frame due to DPB fullness.\n");
#if VERB_ERR_CONC == 1
        NMF_LOG("\nWARNING: Forced output of a frame due to DPB fullness.\n");
#endif

        index = BumpFrame(p_buff);
    }

    /* Check for errors, avoid using a negative index for below tables */
    if (index == -1)
    {
      OstTraceInt0( TRACE_ERROR,"H264DEC: BumpFrame returned -1 at this point, this cannot happen\n");
      NMF_PANIC("BumpFrame returned -1 at this point, this cannot happen\n");
    }

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
  if(cup_context_fifo_push(&p_buff->cup_ctx_desc,p_buff->pics_buf[index].cup_ctx_idx)==-1)
  {
        OstTraceInt0( TRACE_ERROR,"H264DEC: CUP context push failed: fifo full\n");
		  NMF_PANIC("CUP context push failed: fifo full\n");
	}
  p_buff->pics_buf[index].cup_ctx_idx = cup_context_fifo_pop(&p_buff->cup_ctx_desc);
  if(p_buff->pics_buf[index].cup_ctx_idx == -1)
  {
        OstTraceInt0( TRACE_ERROR,"H264DEC: CUP context FIFO empty\n");
   NMF_PANIC("CUP context FIFO empty\n");
  }
#endif

	/* If the frame at index is not available, then we get another one */
	if ((p_buff->pics_buf[index].frame_pos == -1) ||
		(p_buff->frames[p_buff->pics_buf[index].frame_pos].is_available == 0)) {
		//display_queue_remove_pics_buf_frame(p_buff, index);
		mbx = (p_sp->pic_width_in_mbs_minus1+1);
    	mby = (p_sp->pic_height_in_map_units_minus1+1) * (2-p_sp->frame_mbs_only_flag );
	    size16 = ((mbx * mby) << 8)/2;
		p_buff->pics_buf[index].frame_pos = display_queue_get_frame_from_free_pool(p_buff, size16);
		if (p_buff->pics_buf[index].frame_pos == -1) {
		      OstTraceInt0( TRACE_ERROR,"H264DEC: ERROR no new frame available\n");
			NMF_PANIC("\nERROR: no new frame available.\n");
		}
		p_buff->pics_buf[index].ptr = p_buff->frames[p_buff->pics_buf[index].frame_pos].ptr;
	}

#if CLEAN_BUFFER == 1
    {

        t_uint32 size = ((p_sp->pic_width_in_mbs_minus1+1)*(p_sp->pic_height_in_map_units_minus1+1)) << 8;
        (void)memset(p_buff->pics_buf[index].ptr[0], 0, (size_t)(size));
        (void)memset(p_buff->pics_buf[index].ptr[1], 0, (size_t)(size/4));
        (void)memset(p_buff->pics_buf[index].ptr[2], 0, (size_t)(size/4));
    }
#endif

    return index;
}



/**
 * \brief Perform reference picture marking process of last decoded picture
 * \param p_sp Pointer to the active sequence parameter set structure
 * \param p_sh Pointer to the structure with slice header information of last decoded picture
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>Modified interface.</td></tr>
 * </table>
 * </dl>
 *
 * The function performs reference picture marking process including MMCO if needed.
 */

void METH(MarkPicture)(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff)
{
    t_uint16 index = 0;
    t_sint16 i;

    /* IDR picture */
    if (p_sh->nut == 5)
    {
        if (p_sh->long_term_reference_flag == 0)
        {
            p_buff->curr_info->marked_short = 1;
            p_buff->MaxLongTermFrameIdx = -1;
            p_buff->numShortRef++;
        }
        else
        {
            p_buff->curr_info->marked_long = 1;
            p_buff->curr_info->long_ref_id = p_buff->MaxLongTermFrameIdx = 0;
            p_buff->numLongRef++;
        }

        return;
    }

    /* Non IDR picture */

    if (p_sh->nri == 0) /* Picture unused for reference */
        return;

    if (p_sh->adaptive_ref_pic_marking_mode_flag == 0) /* Sliding window */
    {
        p_buff->curr_info->marked_short = 1;
        p_buff->numShortRef++;
        return;
    }

    /* MMCO */
    while (p_sh->memory_management_control_operation[index] != 0)
    {
        switch (p_sh->memory_management_control_operation[index])
        {
            case 1:  /* Unmark short ref */
                i = FindShort(p_buff->DPBsize, p_sh->frame_num - (p_sh->difference_of_pic_nums_minus1[index] + 1), p_buff->pics_buf);
                if (i > -1)
                {
                    p_buff->pics_buf[i].marked_short = 0;
                    p_buff->numShortRef--;
					remove_unmarked_displayed_frames(p_buff, i);
                }
                else if(p_buff->g_seek_mode==0)
                { /* MC: in case of seek mode, MMCO operation may be wrong but it's worth trying */
                     OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: Error in MMCO type 1\n");
#if VERB_ERR_CONC == 1
                    NMF_LOG("Error in MMCO type 1\n");
#endif
                    return;
                }
            break;

            case 2:  /* Unmark long ref */
                i = FindLong(p_buff->DPBsize, p_sh->marking_long_term_pic_num[index], p_buff->pics_buf);
                if (i > -1)
                {
                    p_buff->pics_buf[i].marked_long = 0;
                    p_buff->numLongRef--;
					remove_unmarked_displayed_frames(p_buff, i);
                }
                else if(p_buff->g_seek_mode==0)
                { /* in case of seek mode, MMCO operation may be wrong but it's worth trying */
                     OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: Error in MMCO type 2\n");
#if VERB_ERR_CONC == 1
                    NMF_LOG("Error in MMCO type 2\n");
#endif
                    return;
                }
            break;

            case 3:  /* Assign long ref to short ref */
                i = FindLong(p_buff->DPBsize, p_sh->long_term_frame_idx[index], p_buff->pics_buf);
                if (i > -1)
                {
                    p_buff->pics_buf[i].marked_long = 0;
                    p_buff->numLongRef--;
					remove_unmarked_displayed_frames(p_buff, i);
                }

                i = FindShort(p_buff->DPBsize, p_sh->frame_num - (p_sh->difference_of_pic_nums_minus1[index] + 1), p_buff->pics_buf);
                if (i > -1)
                {
                    p_buff->pics_buf[i].marked_short = 0;
                    p_buff->pics_buf[i].marked_long = 1;
                    p_buff->pics_buf[i].long_ref_id = p_sh->long_term_frame_idx[index];
                    p_buff->numShortRef--;
                    p_buff->numLongRef++;
                }
                else if(p_buff->g_seek_mode==0)
                { /* in case of seek mode, MMCO operation may be wrong but it's worth trying */
                    OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: Error in MMCO type 3\n");
#if VERB_ERR_CONC == 1
                    NMF_LOG("Error in MMCO type 3\n");
#endif
                    return;
                }
            break;

            case 4:  /* Unmark long ref greater than specified */
                for (i = 0; i < p_buff->DPBsize+1; i++)
                {
                    if (p_buff->pics_buf[i].marked_long
                        && (p_buff->pics_buf[i].long_ref_id > (p_sh->max_long_term_frame_idx_plus1[index]-1)))
                    {
                        p_buff->pics_buf[i].marked_long = 0;
                        p_buff->numLongRef--;
						remove_unmarked_displayed_frames(p_buff, i);
                    }
                }

                p_buff->MaxLongTermFrameIdx = p_sh->max_long_term_frame_idx_plus1[index]-1;
            break;

            case 5:  /* Unmark all */
                p_buff->curr_info->need_display = 0; /* Don't display this picture yet */
                /* need_display=0, so in FlushDPB->BumpFrame, is_available will not be reset for this frame */
				FlushDPB(p_buff, p_buff->curr_info->frame_pos);
                p_buff->curr_info->need_display = 1;
                p_buff->DPBfullness++;  /* Count this picture in the DPB */
				/* We flush the whole DPB, excepted the frame we are decoding */

                p_buff->curr_info->frame_num = 0;
                p_buff->curr_info->poc = 0;
                p_buff->PreviousFrameNum = 0;
                p_buff->PrevPicOrderCntMsb = 0;
                p_buff->PrevPicOrderCntLsb = 0; /* p_buff->curr_info->poc; */
                p_buff->FrameNumOffset = 0;
            break;

            case 6:  /* Assign long id to current */
                i = FindLong(p_buff->DPBsize, p_sh->long_term_frame_idx[index], p_buff->pics_buf);
                if (i > -1)
                {
                    p_buff->pics_buf[i].marked_long = 0;
                    p_buff->numLongRef--;
					remove_unmarked_displayed_frames(p_buff, i);
                }

                p_buff->curr_info->marked_long = 1;
                p_buff->curr_info->long_ref_id = p_sh->long_term_frame_idx[index];
                p_buff->numLongRef++;
            break;

            default:
            break;
        }

        index++;
    }

    if (!p_buff->curr_info->marked_long)
    {
        p_buff->curr_info->marked_short = 1;
        p_buff->numShortRef++;
    }

}



/**
 * \brief Generate list 0
 * \param p_sp Pointer to the structure with active sequence parameter set
 * \param p_pp Pointer to the structure with active picture parameter set
 * \param p_sh Pointer to the structure with slice header information
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function generates list 0 performing reordering if necessary.
 */

t_uint16 METH(GenerateList)(t_seq_par *p_sp, t_pic_par *p_pp, t_slice_hdr *p_sh, t_dec_buff *p_buff)
{
    t_sint16 i, j, index, status;
    t_sint32 val;
    t_uint16 num_active_ref, num_active_ref_l1;
    t_uint32 MaxFrameNum = (1 << (p_sp->log2_max_frame_num_minus4 + 4));
    t_sint16 indexes[18], indexes_l1[18];
    t_sint16 intermediate_index, list0_size;

    num_active_ref = p_sh->num_ref_idx_active_override_flag ? p_sh->num_ref_idx_l0_active_minus1 + 1
                                                          : p_pp->num_ref_idx_l0_active_minus1 + 1;
    num_active_ref_l1 = p_sh->num_ref_idx_active_override_flag ? p_sh->num_ref_idx_l1_active_minus1 + 1
                                                               : p_pp->num_ref_idx_l1_active_minus1 + 1;

    if((num_active_ref > 16))
    {
          OstTraceInt0( TRACE_ERROR,"H264DEC: active refs is greater than 16\n");
        NMF_PANIC("ERROR: Active refs is greater than 16\n");
    }

    if ( ((p_sh->slice_type==0) || (p_sh->slice_type==5)) ) /* P pictures */
    {
      for(i=0; i < p_buff->DPBsize+1; i++)
      {
        p_buff->pics_buf[i].ord = 1;
      }

      j = 0;

    /* Short reference ordering */
    do
    {
        status = 0;
        index = -1;
        val = MIN_SINT_32;

        for (i=0; i < p_buff->DPBsize+1; i++)
        {
            if (p_buff->pics_buf[i].ord && p_buff->pics_buf[i].marked_short)
                if ((p_buff->pics_buf[i].pic_num > val))
                {
                    val = p_buff->pics_buf[i].pic_num;
                    index = i;
                }
        }

        if (index > -1)
        {
            indexes[j] = index;
            p_buff->list0[j++] = p_buff->pics_buf[index].ptr;
            p_buff->pics_buf[index].ord = 0;
            status = 1;
        }

    } while (status && (j < p_buff->numShortRef) && (j < num_active_ref));


    /* Long reference ordering */
    status = 1;

    while (status && (j < num_active_ref))
    {
        status = 0;
        index = -1;
        val = MAX_SINT_32;

        for (i=0; i < p_buff->DPBsize+1; i++)
        {
            if (p_buff->pics_buf[i].ord && p_buff->pics_buf[i].marked_long && (p_buff->pics_buf[i].long_ref_id < val))
            {
                val = p_buff->pics_buf[i].long_ref_id;
                index = i;
            }
        }

        if (index>-1)
        {
            indexes[j] = index;
            /* check index value to be more robust */
            if(index!=-1)
            {
              p_buff->list0[j++] = p_buff->pics_buf[index].ptr;
              p_buff->pics_buf[index].ord = 0;
            }
            status = 1;
        }
    }

    if (j == 0)
    {
        t_uint16 k=0;
        index = 0;
        val = p_buff->pics_buf[0].pic_num;

        for(i = 1; i < p_buff->DPBsize+1; i++)
        {
            if ((p_buff->pics_buf[i].pic_num > val) && (p_buff->pics_buf[i].frame_num != p_buff->curr_info->frame_num))
            {
                val = p_buff->pics_buf[i].pic_num;
                index = i;
            }
        }

        /* MC: fill list0 with last reference frame (the one with highest pic_num)
              Note that if the DPB is empty, index 0 is taken by default (in case
              of seek the index zero has been filled with a grey frame) */
        while (k < num_active_ref)
        {
              p_buff->list0[k] = p_buff->pics_buf[index].ptr;
              indexes[k] = index;
              k++;
        }
    OstTraceFiltInst0( H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: Warning List0 is empty, using last ref\n");
#if VERB_ERR_CONC == 1
        NMF_LOG("WARNING: list0 is empty. Using last reference frame\n");
#endif
    }
    else if (j < num_active_ref)
    {
        t_uint16 k = j;
        index = indexes[j-1];

        /* MC: if the list0 is incomplete, fill missing slots with n-1-th index */
        while (k < num_active_ref)
        {
            p_buff->list0[k] = p_buff->pics_buf[index].ptr;  /* modified by MC */
            indexes[k] = index;                              /* modified by MC */
            k++;
        }

        /*num_active_ref = j;*/  /* Avoid error in reordering */
    }

    OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: list0: ");
    for (i=0; i < j; i++)
    {
        OstTraceFiltInst1(H264DEC_TRACE_GROUP_VERB_DPB,"%i",indexes[i]);
    }
    OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"\n");
#if VERB_DPB == 1
    NMF_LOG("\nList0 :");

    for (i=0; i < j; i++)
        NMF_LOG(" %i",indexes[i]);

    NMF_LOG("\n");
#endif

    /* List0 reordering */
    if (p_sh->ref_pic_list_reordering_flag_l0 == 1)
    {
        t_uint16 cIdx, nIdx;
        t_uint16 picNumPred = p_sh->frame_num;

        index = 0;

        while ((p_sh->reordering_of_pic_nums_idc[index] != 3)&&(index<17))
        {
            if (p_sh->reordering_of_pic_nums_idc[index] == 2)
            {
                i = FindLong(p_buff->DPBsize, p_sh->long_term_pic_num[index], p_buff->pics_buf);
                if (i > -1)
                {
                    for (cIdx = num_active_ref; cIdx > index; cIdx--)
                    {
                        p_buff->list0[cIdx] = p_buff->list0[cIdx-1];
                        indexes[cIdx] = indexes[cIdx-1];
                    }

                    p_buff->list0[index] = p_buff->pics_buf[i].ptr;
                    indexes[index] = i;

                    nIdx = index+1;
                    for (cIdx = index+1; cIdx <= num_active_ref; cIdx++)
                        if ((indexes[cIdx] != -1) &&
                            (!p_buff->pics_buf[indexes[cIdx]].marked_long
                            || (p_buff->pics_buf[indexes[cIdx]].long_ref_id != p_sh->long_term_pic_num[index])))
                        {
                            p_buff->list0[nIdx] = p_buff->list0[cIdx];
                            indexes[nIdx++] = indexes[cIdx];
                        }
                }
                else if(p_buff->g_seek_mode==0)
                {/* in case of seek mode this operation can fail but it is worth trying */
                   OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: error reordering list0 (1)\n");
#if VERB_ERR_CONC == 1
                    NMF_LOG("Error reordering list 0\n");
#endif
                    return 1;
                }
            }
            else
            {
                t_sint32 picNumNoWrap, picNum;

                if (p_sh->reordering_of_pic_nums_idc[index] == 0)
                {
                    if ((picNumPred - (p_sh->abs_diff_pic_num_minus1[index] + 1)) < 0)
                        picNumNoWrap = picNumPred - (p_sh->abs_diff_pic_num_minus1[index] + 1) + MaxFrameNum;
                    else
                        picNumNoWrap = picNumPred - (p_sh->abs_diff_pic_num_minus1[index] + 1);
                }
                else
                {
                    if ((t_uint32)(picNumPred + (p_sh->abs_diff_pic_num_minus1[index] + 1)) >= MaxFrameNum)
                        picNumNoWrap = picNumPred + (p_sh->abs_diff_pic_num_minus1[index] + 1) - MaxFrameNum;
                    else
                        picNumNoWrap = picNumPred + (p_sh->abs_diff_pic_num_minus1[index] + 1);
                }

                if (picNumNoWrap > p_sh->frame_num)
                    picNum = picNumNoWrap - MaxFrameNum;
                else
                    picNum = picNumNoWrap;

                picNumPred = picNumNoWrap;

                i = FindShort(p_buff->DPBsize, picNum, p_buff->pics_buf);
                if (i > -1)
                {
                    for (cIdx = num_active_ref; cIdx > index; cIdx--)
                    {
                        p_buff->list0[cIdx] = p_buff->list0[cIdx-1];
                        indexes[cIdx] = indexes[cIdx-1];
                    }

                    p_buff->list0[index] = p_buff->pics_buf[i].ptr;
                    indexes[index] = i;

                    nIdx = index+1;
                    for (cIdx = index+1; cIdx <= num_active_ref; cIdx++)
                        if ((indexes[cIdx] != -1) &&
                            (p_buff->pics_buf[indexes[cIdx]].marked_long || (p_buff->pics_buf[indexes[cIdx]].pic_num != picNum)))
                        {
                            p_buff->list0[nIdx] = p_buff->list0[cIdx];
                            indexes[nIdx++] = indexes[cIdx];
                        }
                }
                else if(p_buff->g_seek_mode==0)
                { /* in case of seek mode this operation can fail but it is worth trying */
                   OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"H264DEC: error reordering list0 (2)\n ");
#if VERB_ERR_CONC == 1
                    NMF_LOG("Error reordering list 0\n");
#endif
                    return 1;
                }
            }

            index++;
        }
        num_active_ref = p_sh->num_ref_idx_active_override_flag ? p_sh->num_ref_idx_l0_active_minus1 + 1
                                                                : p_pp->num_ref_idx_l0_active_minus1 + 1;

        OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"\nReordered List0 :");

        for (i=0; i < num_active_ref; i++)
            OstTraceFiltInst1(H264DEC_TRACE_GROUP_VERB_DPB," %i",indexes[i]);

        OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"\n");

#if VERB_DPB == 1
        NMF_LOG("\nReordered List0 :");

        for (i=0; i < num_active_ref; i++)
            NMF_LOG(" %i",indexes[i]);

        NMF_LOG("\n");
#endif
    }

    num_active_ref = p_sh->num_ref_idx_active_override_flag ? p_sh->num_ref_idx_l0_active_minus1 + 1
                                                            : p_pp->num_ref_idx_l0_active_minus1 + 1;

    for (i = 0; i < num_active_ref; i++)
    {
        p_sh->pic[i] = indexes[i];
    }

         /* added by MC */
    for(i=num_active_ref;i<16;i++ )
    {
      p_sh->pic[i] = 0;
    }
    }  /* P pictures */
    else/* B pictures: (p_sh->slice_type==1) || (p_sh->slice_type==6) */
    {

      if((num_active_ref > 16) || (num_active_ref_l1>16))
      {
            OstTraceInt0( TRACE_ERROR,"H264DEC: active ref greater than 16\n");
        NMF_PANIC("ERROR: Active refs is greater than 16\n");
      }

      t_sint32 this_poc = DecodePOC(p_sp, p_sh, p_buff);

      /* list 0 initialization */
      for(i=0; i < p_buff->DPBsize+1; i++)
      {
          p_buff->pics_buf[i].ord = 1;
      }

      j = 0;

      /* Short reference ordering */
      do
      {
          status = 0;
          index = -1;
          val = MIN_SINT_32;

          /* first part of list 0, forward frames in descending order */
          for (i=0; i < p_buff->DPBsize+1; i++)
          {
              if (p_buff->pics_buf[i].ord && p_buff->pics_buf[i].marked_short)
                  if ( (p_buff->pics_buf[i].poc > val) && (this_poc >= p_buff->pics_buf[i].poc) ) /* >= just for error robustness */
                  {
                      val = p_buff->pics_buf[i].poc;
                      index = i;
                  }
          }

          if (index > -1)
          {
              indexes[j] = index;
              p_buff->list0[j++] = p_buff->pics_buf[index].ptr;
              p_buff->pics_buf[index].ord = 0;
              status = 1;
          }
      } while (status && (j < p_buff->numShortRef));

      intermediate_index = j;

      do
      {
          status = 0;
          index = -1;
          val = MAX_SINT_32;

          /* second part of list 0, backward frames in ascending order */
          for (i=0; i < p_buff->DPBsize+1; i++)
          {
              if (p_buff->pics_buf[i].ord && p_buff->pics_buf[i].marked_short)
                  if ( (p_buff->pics_buf[i].poc < val) && (this_poc < p_buff->pics_buf[i].poc) )
                  {
                      val = p_buff->pics_buf[i].poc;
                      index = i;
                  }
          }

          if (index > -1)
          {
              indexes[j] = index;
              p_buff->list0[j++] = p_buff->pics_buf[index].ptr;
              p_buff->pics_buf[index].ord = 0;
              status = 1;
          }

      } while (status && (j < p_buff->numShortRef));

      list0_size = j;

      /* now we have list0 and intermediate_index, we can infer list1 */
      for ( i = 0; i < intermediate_index; i++ )
      {
        p_buff->list1[list0_size-intermediate_index+i] = p_buff->list0[i];
        indexes_l1[list0_size-intermediate_index+i] = indexes[i];
      }
      for ( i = intermediate_index; i < list0_size; i++ )
      {
        p_buff->list1[i-intermediate_index] = p_buff->list0[i];
        indexes_l1[i-intermediate_index] = indexes[i];
      }

      /* Long reference ordering - append long term pictures at the end of both list0 and list1 in ascending order */
      status = 1;
      while (status)
      {
          status = 0;
          val = p_buff->MaxLongTermFrameIdx+1;

          for (i=0; i < p_buff->DPBsize+1; i++)
          {
              if (p_buff->pics_buf[i].ord && p_buff->pics_buf[i].marked_long && (p_buff->pics_buf[i].long_ref_id < val))
              {
                  val = p_buff->pics_buf[i].long_ref_id;
                  index = i;
              }
          }

          if (val < p_buff->MaxLongTermFrameIdx+1)
          {
              indexes[j] = indexes_l1[j] = index;
              p_buff->list0[j  ] = p_buff->pics_buf[index].ptr;
              p_buff->list1[j++] = p_buff->pics_buf[index].ptr;
              p_buff->pics_buf[index].ord = 0;
              status = 1;
          }
      }

      /* last, check if the 2 lists are equal; if they are, swap the first 2 elements of list1 */
      if ( j > 1 )
      {
        t_uint16 diff = 0;
        for ( i = 0; i < j; i++ )
        {
          if ( indexes[i] != indexes_l1[i] )
            diff = 1;
        }
        if ( !diff )
        {
          t_sint16 tmp_index;
          t_uint16 **tmp_ptr;

          tmp_ptr = p_buff->list1[0];
          p_buff->list1[0] = p_buff->list1[1];
          p_buff->list1[1] = tmp_ptr;

          tmp_index = indexes_l1[0];
          indexes_l1[0] = indexes_l1[1];
          indexes_l1[1] = tmp_index;
        }
      }

      /* check errors */
      if (j == 0)
      {
          t_uint16 k=0;
          index = 0;
          val = p_buff->pics_buf[0].pic_num;

          for(i = 1; i < p_buff->DPBsize+1; i++)
          {
              if ((p_buff->pics_buf[i].pic_num > val) && (p_buff->pics_buf[i].frame_num != p_buff->curr_info->frame_num))
              {
                  val = p_buff->pics_buf[i].pic_num;
                  index = i;
              }
          }

          /* MC: fill list0/1 with last reference frame (the one with highest pic_num)
              Note that if the DPB is empty, index 0 is taken by default (in case
              of seek the index zero has been filled with a grey frame) */
          while ((k < num_active_ref)||(k < num_active_ref_l1))
          {
              p_buff->list0[k] = p_buff->pics_buf[index].ptr;
              p_buff->list1[k] = p_buff->pics_buf[index].ptr;
              indexes[k] = index;
              indexes_l1[k] = index;
              k++;
          }

        OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"WARNING: list0 and list1 are empty. Using last reference frame\n");
#if VERB_ERR_CONC == 1
          NMF_LOG("WARNING: list0 and list1 are empty. Using last reference frame\n");
#endif
      }
      else
      {
        if (j < num_active_ref)
        {
            t_uint16 k = j;
            index = indexes[j-1];

            /* MC: if the list0 is incomplete, fill missing slots with n-1-th index */
            while (k < num_active_ref)
            {
              p_buff->list0[k] = p_buff->pics_buf[index].ptr;
              indexes[k] = index;
              k++;
            }

            /*num_active_ref = j;*/  /* Avoid error in reordering */
        }
        if (j < num_active_ref_l1)
        {
            t_uint16 k = j;
            index = indexes_l1[j-1];

            /* MC: if the list1 is incomplete, fill missing slots with n-1-th index */
            while (k < num_active_ref_l1)
            {
              p_buff->list1[k] = p_buff->pics_buf[index].ptr;
              indexes_l1[k] = index;
              k++;
            }

            /*num_active_ref = j;*/  /* Avoid error in reordering */
        }
      }

      OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"List0 B picture :");
      for (i=0; i < j; i++)
          OstTraceFiltInst1(H264DEC_TRACE_GROUP_VERB_DPB," %i",indexes[i]);
      OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"\n");
      OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"List1 B picture :");
      for (i=0; i < j; i++)
          OstTraceFiltInst1(H264DEC_TRACE_GROUP_VERB_DPB," %i",indexes_l1[i]);
      OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"\n");

#if VERB_DPB == 1
      NMF_LOG("\nList0 B picture :");

      for (i=0; i < j; i++)
          NMF_LOG(" %i",indexes[i]);

      NMF_LOG("\n");

      NMF_LOG("\nList1 B picture :");

      for (i=0; i < j; i++)
          NMF_LOG(" %i",indexes_l1[i]);

      NMF_LOG("\n");
#endif

      /* List0 reordering */
      if ((p_sh->ref_pic_list_reordering_flag_l0 == 1))
      {
          t_uint16 cIdx, nIdx;
          t_uint16 picNumPred = p_sh->frame_num;

          index = 0;

          while ((p_sh->reordering_of_pic_nums_idc[index] != 3)&&(index<17))
          {
              if (p_sh->reordering_of_pic_nums_idc[index] == 2)
              {
                  i = FindLong(p_buff->DPBsize, p_sh->long_term_pic_num[index], p_buff->pics_buf);
                  if (i > -1)
                  {
                      for (cIdx = num_active_ref; cIdx > index; cIdx--)
                      {
                          p_buff->list0[cIdx] = p_buff->list0[cIdx-1];
                          indexes[cIdx] = indexes[cIdx-1];
                      }

                      p_buff->list0[index] = p_buff->pics_buf[i].ptr;
                      indexes[index] = i;

                      nIdx = index+1;
                      for (cIdx = index+1; cIdx <= num_active_ref; cIdx++)
                          if ((indexes[cIdx] != -1) &&
                              (!p_buff->pics_buf[indexes[cIdx]].marked_long
                              || (p_buff->pics_buf[indexes[cIdx]].long_ref_id != p_sh->long_term_pic_num[index])))
                          {
                              p_buff->list0[nIdx] = p_buff->list0[cIdx];
                              indexes[nIdx++] = indexes[cIdx];
                          }
                  }
                  else if(p_buff->g_seek_mode==0)
                  {/* in case of seek mode this operation can fail but it is worth trying */
                      OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"B slice 2:Error reordering list 0\n");
#if VERB_ERR_CONC == 1
                      NMF_LOG("B slice 2:Error reordering list 0\n");
#endif
                      return 1;
                  }
              }
              else
              {
                  t_sint32 picNumNoWrap, picNum;

                  if (p_sh->reordering_of_pic_nums_idc[index] == 0)
                  {
                      if ((picNumPred - (p_sh->abs_diff_pic_num_minus1[index] + 1)) < 0)
                          picNumNoWrap = picNumPred - (p_sh->abs_diff_pic_num_minus1[index] + 1) + MaxFrameNum;
                      else
                          picNumNoWrap = picNumPred - (p_sh->abs_diff_pic_num_minus1[index] + 1);
                  }
                  else
                  {
                      if ((t_uint32)(picNumPred + (p_sh->abs_diff_pic_num_minus1[index] + 1)) >= MaxFrameNum)
                          picNumNoWrap = picNumPred + (p_sh->abs_diff_pic_num_minus1[index] + 1) - MaxFrameNum;
                      else
                          picNumNoWrap = picNumPred + (p_sh->abs_diff_pic_num_minus1[index] + 1);
                  }

                  if (picNumNoWrap > p_sh->frame_num)
                      picNum = picNumNoWrap - MaxFrameNum;
                  else
                      picNum = picNumNoWrap;

                  picNumPred = picNumNoWrap;

                  i = FindShort(p_buff->DPBsize, picNum, p_buff->pics_buf);
                  if (i > -1)
                  {
                      for (cIdx = num_active_ref; cIdx > index; cIdx--)
                      {
                          p_buff->list0[cIdx] = p_buff->list0[cIdx-1];
                          indexes[cIdx] = indexes[cIdx-1];
                      }

                      p_buff->list0[index] = p_buff->pics_buf[i].ptr;
                      indexes[index] = i;

                      nIdx = index+1;
                      for (cIdx = index+1; cIdx <= num_active_ref; cIdx++)
                          if ((indexes[cIdx] != -1) &&
                              (p_buff->pics_buf[indexes[cIdx]].marked_long || (p_buff->pics_buf[indexes[cIdx]].pic_num != picNum)))
                          {
                              p_buff->list0[nIdx] = p_buff->list0[cIdx];
                              indexes[nIdx++] = indexes[cIdx];
                          }
                  }
                  else if(p_buff->g_seek_mode==0)
                  {/* in case of seek mode this operation can fail but it is worth trying */
                      OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"B slice 2:Error reordering list 0(2)\n");
#if VERB_ERR_CONC == 1
                      NMF_LOG("B slice: Error reordering list 0\n");
#endif
                      return 1;
                  }
              }

              index++;
          }
          num_active_ref = p_sh->num_ref_idx_active_override_flag ? p_sh->num_ref_idx_l0_active_minus1 + 1
                                                                  : p_pp->num_ref_idx_l0_active_minus1 + 1;

          OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"Reordered List0 :");
          for (i=0; i < num_active_ref; i++)
              OstTraceFiltInst1(H264DEC_TRACE_GROUP_VERB_DPB," %i",indexes[i]);
          OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"\n");

#if VERB_DPB == 1
          NMF_LOG("\nReordered List0 :");

          for (i=0; i < num_active_ref; i++)
              NMF_LOG(" %i",indexes[i]);

          NMF_LOG("\n");
#endif
      }

      num_active_ref = p_sh->num_ref_idx_active_override_flag ? p_sh->num_ref_idx_l0_active_minus1 + 1
                                                              : p_pp->num_ref_idx_l0_active_minus1 + 1;

      for (i = 0; i < num_active_ref; i++)
      {
          p_sh->pic[i] = indexes[i];
      }

      /* added by MC */
      for(i=num_active_ref;i<16;i++ )
      {
        p_sh->pic[i] = 0;
      }

      /* List1 reordering */
      if ((p_sh->ref_pic_list_reordering_flag_l1 == 1))
      {
          t_uint16 cIdx, nIdx;
          t_uint16 picNumPred = p_sh->frame_num;

          index = 0;

          while ((index<17) && (p_sh->reordering_of_pic_nums_idc_l1[index] != 3))
          {
              if (p_sh->reordering_of_pic_nums_idc_l1[index] == 2)
              {
                  i = FindLong(p_buff->DPBsize, p_sh->long_term_pic_num_l1[index], p_buff->pics_buf);
                  if (i > -1)
                  {
                      for (cIdx = num_active_ref_l1; cIdx > index; cIdx--)
                      {
                          p_buff->list1[cIdx] = p_buff->list1[cIdx-1];
                          indexes_l1[cIdx] = indexes_l1[cIdx-1];
                      }

                      p_buff->list1[index] = p_buff->pics_buf[i].ptr;
                      indexes_l1[index] = i;

                      nIdx = index+1;
                      for (cIdx = index+1; cIdx <= num_active_ref_l1; cIdx++)
                          if ((indexes_l1[cIdx] != -1) &&
                              (!p_buff->pics_buf[indexes_l1[cIdx]].marked_long
                              || (p_buff->pics_buf[indexes_l1[cIdx]].long_ref_id != p_sh->long_term_pic_num_l1[index])))
                          {
                              p_buff->list1[nIdx] = p_buff->list1[cIdx];
                              indexes_l1[nIdx++] = indexes_l1[cIdx];
                          }
                  }
                  else if(p_buff->g_seek_mode==0)
                  {/* in case of seek mode this operation can fail but it is worth trying */
                      OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"B slice 2:Error reordering list 1\n");
#if VERB_ERR_CONC == 1
                      NMF_LOG("B slice : Error reordering list 1\n");
#endif
                      return 1;
                  }
              }
              else
              {
                  t_sint32 picNumNoWrap, picNum;

                  if (p_sh->reordering_of_pic_nums_idc_l1[index] == 0)
                  {
                      if ((picNumPred - (p_sh->abs_diff_pic_num_minus1_l1[index] + 1)) < 0)
                          picNumNoWrap = picNumPred - (p_sh->abs_diff_pic_num_minus1_l1[index] + 1) + MaxFrameNum;
                      else
                          picNumNoWrap = picNumPred - (p_sh->abs_diff_pic_num_minus1_l1[index] + 1);
                  }
                  else
                  {
                      if ((t_uint32)(picNumPred + (p_sh->abs_diff_pic_num_minus1_l1[index] + 1)) >= MaxFrameNum)
                          picNumNoWrap = picNumPred + (p_sh->abs_diff_pic_num_minus1_l1[index] + 1) - MaxFrameNum;
                      else
                          picNumNoWrap = picNumPred + (p_sh->abs_diff_pic_num_minus1_l1[index] + 1);
                  }

                  if (picNumNoWrap > p_sh->frame_num)
                      picNum = picNumNoWrap - MaxFrameNum;
                  else
                      picNum = picNumNoWrap;

                  picNumPred = picNumNoWrap;

                  i = FindShort(p_buff->DPBsize, picNum, p_buff->pics_buf);
                  if (i > -1)
                  {
                      for (cIdx = num_active_ref_l1; cIdx > index; cIdx--)
                      {
                          p_buff->list1[cIdx] = p_buff->list1[cIdx-1];
                          indexes_l1[cIdx] = indexes_l1[cIdx-1];
                      }

                      p_buff->list1[index] = p_buff->pics_buf[i].ptr;
                      indexes_l1[index] = i;

                      nIdx = index+1;
                      for (cIdx = index+1; cIdx <= num_active_ref_l1; cIdx++)
                          if ((indexes_l1[cIdx] != -1) &&
                              (p_buff->pics_buf[indexes_l1[cIdx]].marked_long || (p_buff->pics_buf[indexes_l1[cIdx]].pic_num != picNum)))
                          {
                              p_buff->list1[nIdx] = p_buff->list1[cIdx];
                              indexes_l1[nIdx++] = indexes_l1[cIdx];
                          }
                  }
                  else if(p_buff->g_seek_mode==0)
                  { /* in case of seek mode this operation can fail but it is worth trying */
                      OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"B slice 2:Error reordering list 1(2)\n");
#if VERB_ERR_CONC == 1
                      NMF_LOG("Error reordering list 1\n");
#endif
                      return 1;
                  }
              }

              index++;
          }
          num_active_ref_l1 = p_sh->num_ref_idx_active_override_flag ? p_sh->num_ref_idx_l1_active_minus1 + 1
                                                                     : p_pp->num_ref_idx_l1_active_minus1 + 1;


          OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"Reordered List1 :");
          for (i=0; i < num_active_ref_l1; i++)
              OstTraceFiltInst1(H264DEC_TRACE_GROUP_VERB_DPB," %i",indexes_l1[i]);
          OstTraceFiltInst0(H264DEC_TRACE_GROUP_VERB_DPB,"\n");

#if VERB_DPB == 1
          NMF_LOG("\nReordered List1 :");

          for (i=0; i < num_active_ref_l1; i++)
              NMF_LOG(" %i",indexes_l1[i]);

          NMF_LOG("\n");
#endif
      }

      num_active_ref_l1 = p_sh->num_ref_idx_active_override_flag ? p_sh->num_ref_idx_l1_active_minus1 + 1
                                                                 : p_pp->num_ref_idx_l1_active_minus1 + 1;

      for (i = 0; i < num_active_ref_l1; i++)
      {
          p_sh->pic_l1[i] = indexes_l1[i];
      }
      /* added by MC */
      for(i=num_active_ref_l1;i<16;i++ )
      {
        p_sh->pic_l1[i] = 0;
      }

    } /*  B frames */

    return 0;
}



/**
 * \brief Find a frame in the DPB with the selected short reference id
 * \param DPBsize Size of the DPB
 * \param short_id Short reference id to find
 * \param p_pics_buf Pointer to the decoded picture buffer structure
 * \return DPB index of the picture with the selected short reference id. -1 if not found.
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function finds a frame with the selected short reference id in the DPB.
 */

static t_sint16 FindShort(t_uint16 DPBsize, t_sint32 short_id, t_dec_pic_info *p_pics_buf)
{
    t_uint16 i;

    for (i = 0; i < DPBsize+1; i++)
        if (p_pics_buf[i].marked_short && (p_pics_buf[i].pic_num == short_id))
            return i;

    return -1;
}



/**
 * \brief Find a frame with the selected long reference id
 * \param DPBsize Size of the DPB
 * \param long_id Long reference id to find
 * \param p_pics_buf Pointer to the decoded picture buffer structure
 * \return DPB index of the picture with the selected long reference id. -1 if not found.
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function find a frame with the selected long reference id in the DPB.
 */

static t_sint16 FindLong(t_uint16 DPBsize, t_uint16 long_id, t_dec_pic_info *p_pics_buf)
{
    t_uint16 i;

    for (i = 0; i < DPBsize+1; i++)
        if (p_pics_buf[i].marked_long && (p_pics_buf[i].long_ref_id == long_id))
            return i;

    return -1;
}



/**
 * \brief Decode Picture Order Count (POC)
 * \param p_sp Pointer to the active sequence parameter set structure
 * \param p_sh Pointer to the structure with current slice header information
 * \param p_buff Pointer to the buffers structure
 * \return POC of current picture
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function decodes Picture Order Count.
 */

static t_sint32 DecodePOC(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff)
{
    t_uint16 i;
    t_uint32 MaxPicOrderCntLsb, AbsFrameNum, FrameNumInPicOrderCntCycle, PicOrderCntCycleCnt;
    t_sint32 poc=0, toppoc, bottompoc, PicOrderCntMsb, ExpectedDeltaPerPicOrderCntCycle, ExpectedPicOrderCnt;

    switch (p_sp->pic_order_cnt_type)
    {
        case 0:
            MaxPicOrderCntLsb = (1 << (p_sp->log2_max_pic_order_cnt_lsb_minus4 + 4));

            if( p_sh->pic_order_cnt_lsb  <  p_buff->PrevPicOrderCntLsb  &&
                    ( p_buff->PrevPicOrderCntLsb - p_sh->pic_order_cnt_lsb )  >=  (t_uint16)( MaxPicOrderCntLsb >> 1 ) )
                PicOrderCntMsb = p_buff->PrevPicOrderCntMsb + MaxPicOrderCntLsb;
            else if ( p_sh->pic_order_cnt_lsb  >  p_buff->PrevPicOrderCntLsb  &&
                    ( p_sh->pic_order_cnt_lsb - p_buff->PrevPicOrderCntLsb )  >  (t_uint16)( MaxPicOrderCntLsb >> 1 ) )
                PicOrderCntMsb = p_buff->PrevPicOrderCntMsb - MaxPicOrderCntLsb;
            else
                PicOrderCntMsb = p_buff->PrevPicOrderCntMsb;

            toppoc = PicOrderCntMsb + p_sh->pic_order_cnt_lsb;
            bottompoc = toppoc + p_sh->delta_pic_order_cnt_bottom;

            poc = MIN(toppoc, bottompoc);

            if (p_sh->nri != 0)
            {
                p_buff->PrevPicOrderCntLsb = p_sh->pic_order_cnt_lsb;
                p_buff->PrevPicOrderCntMsb = PicOrderCntMsb;
            }
        break;

        case 1:
            if (p_buff->PreviousFrameNum > p_sh->frame_num)
                p_buff->FrameNumOffset += (1 << (p_sp->log2_max_frame_num_minus4 + 4));

            if(p_sp->num_ref_frames_in_pic_order_cnt_cycle)
                AbsFrameNum = p_buff->FrameNumOffset + p_sh->frame_num;
            else
                AbsFrameNum = 0;

            if((p_sh->nri == 0) && (AbsFrameNum > 0))
                AbsFrameNum--;

            ExpectedDeltaPerPicOrderCntCycle = 0;

            if(p_sp->num_ref_frames_in_pic_order_cnt_cycle)
                for(i = 0; i < p_sp->num_ref_frames_in_pic_order_cnt_cycle; i++)
                    ExpectedDeltaPerPicOrderCntCycle += p_sp->offset_for_ref_frame[i];

            if(AbsFrameNum)
            {
                PicOrderCntCycleCnt = (AbsFrameNum - 1) / p_sp->num_ref_frames_in_pic_order_cnt_cycle;
                FrameNumInPicOrderCntCycle = (AbsFrameNum - 1) % p_sp->num_ref_frames_in_pic_order_cnt_cycle;
                ExpectedPicOrderCnt = PicOrderCntCycleCnt * ExpectedDeltaPerPicOrderCntCycle;

                for(i = 0; i <= FrameNumInPicOrderCntCycle; i++)
                    ExpectedPicOrderCnt += p_sp->offset_for_ref_frame[i];
            }
            else
                ExpectedPicOrderCnt = 0;

            if(p_sh->nri == 0)
                ExpectedPicOrderCnt += p_sp->offset_for_non_ref_pic;

            toppoc = ExpectedPicOrderCnt + p_sh->delta_pic_order_cnt[0];
            bottompoc = toppoc + p_sp->offset_for_top_to_bottom_field + p_sh->delta_pic_order_cnt[1];

            poc = (toppoc < bottompoc) ? toppoc : bottompoc;
        break;

        case 2:
            if (p_buff->PreviousFrameNum > p_sh->frame_num)
                p_buff->FrameNumOffset += (1 << (p_sp->log2_max_frame_num_minus4 + 4));

            if (p_sh->nut == 5)
                poc = 0;
            else if (p_sh->nri == 0)
                poc = 2 * (p_buff->FrameNumOffset + p_sh->frame_num) - 1;
            else
                poc = 2 * (p_buff->FrameNumOffset + p_sh->frame_num);
        break;

        default:
        break;
    }

    return poc;
}



/**
 * \brief Conceal a frame
 * \param prevNum Aspected frame number
 * \param pos Position of DPB where to insert the concealed frame
 * \param sp Pointer to sequence parameter set
 * \param p_buff Pointer to the decoder buffer structure
 * \return 0 on success, 1 if impossible to conceal
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function conceal a frame by replicating the temporal nearest (highest POC) frame in the DPB.
 */

t_uint16 METH(ConcealFrame)(t_uint16 prevNum, t_uint16 pos, t_seq_par *sp, t_dec_buff *p_buff)
{
    t_sint32 poc = MIN_SINT_32;
    t_sint16 index;
    t_uint16 i;
    t_dec_pic_info *p_dpi;

    /* find the highest poc in marked frames */
	 index = -1;
    for (i = 0; i < p_buff->DPBsize+1; i++)
    {
        if (((p_buff->pics_buf[i].marked_short) || (p_buff->pics_buf[i].marked_long)) && (p_buff->pics_buf[i].poc > poc))
        {
            poc = p_buff->pics_buf[i].poc;
            index = i;
        }
    }

    /* no candidate found , return error */
    if (index == -1)	return 1;



	/* Duplicate the information from DPB location  		*/
	/* Two (or more) DPB location point to the same frame	*/

    /* Set frame infos */
	p_dpi = &p_buff->pics_buf[pos];

    /* set buffer pointer */
	p_dpi->ptr = p_buff->pics_buf[index].ptr;
	p_dpi->frame_pos = p_buff->pics_buf[index].frame_pos;


    /* Initialize frame infos */
    p_dpi->frame_num = prevNum;
    p_dpi->frame_num_wrap = p_dpi->pic_num = p_dpi->frame_num;
    p_dpi->marked_short = 1;
    p_dpi->marked_long = 0;
    p_dpi->need_display = 0; /* Concealed frames are not displayed, so screen freeze */
    p_dpi->poc = poc + 1;

	/* increase DPB buffer fullness */
    p_buff->numShortRef++;
    p_buff->DPBfullness++;

    if(p_buff->g_decoded_frm_ctr==0)
       dec_static.g_last_timestamp = 0;
    p_buff->g_decoded_frm_ctr++; // for timestamps generation

    return 0;
}

t_sint16 METH(is_marked_frame)(t_dec_buff *p_buff, t_dpb_info *dpb_info)
{
	if ((dpb_info->marked_short == 1) || (dpb_info->marked_long == 1)) return 1;
	return 0;
}

/**
 * \brief search if frame is marked as referenced, or need to be displayed, in the DPB
 * for this purpose all DPB location are tested to see if the frame used by the
 * current DPB location match the searched one
 * if yes, then it check if DPB location is still used (marked or need_display)
 */
t_sint16 METH(is_dpbused_frame)(t_dec_buff *p_buff, t_frames *frame) {
	int j;
	// scanning all frame in DPB
	for (j=0; j < p_buff->DPBsize+1; j++)
	{
		t_dec_pic_info	*pic_info=&p_buff->pics_buf[j];

		// DPB location use the same frame as the search one
		if	( ( pic_info->frame_pos>=0) && (frame == &p_buff->frames[pic_info->frame_pos] ))
		{
			if ((pic_info->marked_short == 1) || (pic_info->marked_long == 1) ||
				(pic_info->need_display == 1))  return 1;
		}
	}
	return 0;
}

void METH(remove_unmarked_displayed_frames)(t_dec_buff *p_buff, t_sint16 pos_dpb) {
	// If the frame at that dpb pos don't need to be displayed, and is not marked
	// then it must be moved to the display queue
	if ((pos_dpb != -1) &&
		(p_buff->pics_buf[pos_dpb].marked_short == 0) && (p_buff->pics_buf[pos_dpb].marked_long == 0) &&
		(p_buff->pics_buf[pos_dpb].need_display == 0)) {
		/* In case of frame concealement, we must check first if the frame at pos_dpb is not
		 * a frame that still need to be displayed in another dpb location before removing it
		 * => check need_display in display_queue_remove_unreferenced_frame->is_dpbused_frame
		 * (frame 95 in erc_22) */
		display_queue_remove_unreferenced_frame(p_buff, pos_dpb);
	}
}


/**
 * \brief Dump the DPB for debugging
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function dump the DPB for debugging.
 */

#if VERB_DPB == 1
void DumpDPB(t_dec_buff *p_buff)
{
#if VERB_DPB == 2
	t_uint16 i;

    NMF_LOG("\nPOS \t Short \t Long \t Display \t Frame Num \t Frame Num Wrap \t Pic Num \t Long ref \n");

    for (i = 0; i < p_buff->DPBsize+1; i++)
    {
        NMF_LOG("%i \t %i \t %i \t %i \t %i \t\t %li \t\t\t %li \t\t %i\n",  i,
                                                            p_buff->pics_buf[i].marked_short,
                                                            p_buff->pics_buf[i].marked_long,
                                                            p_buff->pics_buf[i].need_display,
                                                            p_buff->pics_buf[i].frame_num,
                                                            p_buff->pics_buf[i].frame_num_wrap,
                                                            p_buff->pics_buf[i].pic_num,
                                                            p_buff->pics_buf[i].long_ref_id);
    }
#endif
}
#endif

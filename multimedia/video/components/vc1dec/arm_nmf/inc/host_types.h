/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

 
#ifndef HOST_TYPES_H
#define HOST_TYPES_H

#include "types.h"
#include "settings.h"
#include "frame.h"


/**
 * \brief Structure containing decoded pictures info
 *    This is one picture in the decoder's DPB
 */
typedef struct
{
    t_uint8 **ptr;              // this points to frames[frame_pos].ptr
	t_sint16 frame_pos;         // position of this frame in the "frames" array (phys pointers)
    t_uint16 marked;            // indicates that current picture is used as ref
    t_uint32 nFlags;            // OMX flags, from input to output
    t_uint16 need_display;
} t_dec_pic_info;

typedef t_dec_pic_info t_dpb_info;

/**
 * \brief Structure containing buffers and relatives info
 */
typedef struct 
{
    t_frames *frames;            // the frame buffers in the display queue
	  t_uint16 max_nb_frames;      // size of the above array

    /* Array containing information relatives to pictures in the decoding buffer */
    t_dec_pic_info pics_buf[MAXNUMFRM];	
	  t_uint16       oldest_ref;   // index of the oldest reference (either 0 or 1)	 
    t_dec_pic_info *curr_info;
        
    /* Variables for DPB management */
    t_uint16 DPBsize;
    t_uint16 initialized; 
               
} t_dec_buff;

 /**
 * \brief auxiliary variables to prepare parameters for Hamac
 */
typedef struct
{
    t_uint8 * fwd_ref_frame;
    t_uint8 * bwd_ref_frame;
    t_uint8 * curr_frame;
    t_uint8 * p_deblocking_paramv;
    t_uint8 * local_recon_buff;
    t_uint8 * p_mv_history_buf;
    t_uint8 * aux_frame;
    t_uint8 * cup_context;
    t_bit_buffer * curr_bitstream;
    t_uint32 p_param_inout;
}  t_sva_buffers;

 
#endif

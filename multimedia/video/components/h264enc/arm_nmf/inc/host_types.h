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
#if ENABLE_VUI == 1
    #include "host_vui.h"
#endif



/**
 * \brief Structure containing dpb+output fifo buffer information
 */

typedef struct
{
	t_sint16 wait_tobe_free;
	t_uint16 is_available;
	t_sint16 pic_buf_index[MAXNUMFRM]; /* pic_puf indexes that use this frame (-1=none) */
	t_uint16 *ptr[3];
} t_frames;


/**
 * \brief Structure containing decoded pictures info
 */

typedef struct
{
    t_uint16 **ptr;
    t_sint16 frame_pos;
    t_uint16 marked_short;
    t_uint16 marked_long;
    t_uint16 frame_num;
    t_sint32 frame_num_wrap;
    t_sint32 pic_num;
    t_uint16 long_ref_id;
    t_sint32 poc;
    t_uint16 need_display;
    t_uint16 ord;  /* Auxiliary flag used for ordering */
    
#if ENABLE_TIMING == 1
    double display_time;
    t_uint32 nTimeStampH;       // High part of the ticks as seen in the input buffer (cf. omx specification)
    t_uint32 nTimeStampL;       // Low  part of theticks as seen in the input buffer (cf. omx specification)
#endif
    t_uint32 nFlags;            // OMX flags, from input to output
} t_dec_pic_info;



/**
 * \brief Structure containing buffers and relatives info
 */

typedef struct 
{
    /* Buffer for block infos */
    t_block_info *b_info;

    /* Array for MB to Slice Group Map */
    t_uint16 *mb_slice_map;

    /* Frame buffers = initial allocated memory */
    t_frames *frames;
    t_uint16 max_nb_frames;

    /* Auxiliary buffer for deblocking filter */
    t_uint16 *aux_frame;

    /* Array for deblocking filter parameter */
    tps_h4d_param *p_deblocking_paramv;

    /* ... */
    void *addr_h264e_H4D_buffer;
    void *addr_dest_buffer;
    void *addr_intra_refresh_buffer;
    void *addr_motion_vector_buffer;
    void *addr_search_window_buffer;
    void *addr_ref_frame;
    void *addr_source_buffer;
    void *addr_bitstream_buffer;
#if defined  __ndk8500_a0__ || defined __ndk5500_a0__
    void *addr_h264e_rec_local;
    void *addr_h264e_cup_context;
#endif
#ifdef __ndk5500_a0__
    void *addr_external_cwi;
    void *addr_sequence_info_buffer;
#endif
    t_uint32 bs_size;
    t_uint32 sw_size;

    /* Pointer to current frame buffer */
    t_uint16 **curr_frame;
    t_dec_pic_info *curr_info;

    /* Variables fo POC decoding */
    t_uint32 FrameNumOffset;
    t_sint32 PrevPicOrderCntMsb;
    t_uint16 PrevPicOrderCntLsb;

    /* Array containing information relatives to pictures in the decoding buffer */
    t_dec_pic_info pics_buf[MAXNUMFRM];

    t_uint16 PicSizeInMbs;

    /* Information about concealment done by Hamac */
    t_uint16 intra_conc;
    t_address ref_frame;
    /* Information about DBLK mode done by Hamac (optimized or not)*/
    t_uint16	DBLK_mode;
} t_dec_buff;




/**
 * \brief Structure containing old slice parameters 
 */
 
typedef struct
{
   t_uint16 bottom_field_flag;
   t_uint16 frame_num;
   t_uint16 nal_ref_idc;
   t_uint16 pic_order_cnt_lsb;
   t_sint32 delta_pic_order_cnt_bottom;
   t_sint32 delta_pic_order_cnt[2];
   t_uint16 idr_flag;
   t_uint16 idr_pic_id;
   t_uint16 pps_id;
   t_uint16 redundant_pic_cnt;
   t_uint16 mmco5_flag;  
   t_uint16 error_type;
     
} t_old_slice;




/**
 * \brief Structure containing sequence parameter set information 
 */
 
typedef struct
{
    t_uint16 profile_idc;
    t_uint16 constraint_flags;
    t_uint16 level_idc;
    t_uint16 seq_par_set_id;
    t_uint16 log2_max_frame_num_minus4;
    t_uint16 pic_order_cnt_type;
    t_uint16 log2_max_pic_order_cnt_lsb_minus4;
    t_uint16 delta_pic_order_always_zero_flag;
    t_sint32 offset_for_non_ref_pic;
    t_sint32 offset_for_top_to_bottom_field;
    t_uint16 num_ref_frames_in_pic_order_cnt_cycle;
    t_sint32 offset_for_ref_frame[256];
    t_uint16 num_ref_frames;
    t_uint16 gaps_in_frame_num_value_flag;
    t_uint16 pic_width_in_mbs_minus1;
    t_uint16 pic_height_in_map_units_minus1;
    t_uint16 frame_mbs_only_flag;
    t_uint16 mb_adaptive_frame_field_flag;
    t_uint16 direct_8x8_inf_flag;
    t_uint16 frame_cropping_flag;
    t_uint16 frame_crop_left;
    t_uint16 frame_crop_right;
    t_uint16 frame_crop_top;
    t_uint16 frame_crop_bottom;
    t_uint16 vui_pars_flag;
    
#if ENABLE_VUI == 1
    t_vui_data vui;
#endif
} t_seq_par; /* 560 bytes */




/**
 * \brief Structure containing picture parameter set information
 */
 
typedef struct
{
    t_uint16 pic_par_set_id;
    t_uint16 seq_par_set_id;
    t_uint16 entropy_mode_flag;
    t_uint16 pic_order_present_flag;
    t_uint16 num_slice_groups_minus1;
    t_uint16 slice_group_map_type;
    t_uint16 run_length_minus1[8];
    t_uint16 top_left[8];
    t_uint16 bottom_right[8];
    t_uint16 slice_group_change_dir_flag;
    t_uint16 slice_group_change_rate_minus1;
    t_uint16 pic_size_in_map_units_minus1;
    t_uint16 slice_group_id[1620];
    t_uint16 num_ref_idx_l0_active_minus1;
    t_uint16 num_ref_idx_l1_active_minus1;
    t_uint16 weighted_pred_flag;
    t_uint16 weighted_bipred_idc;
    t_sint16 pic_init_qp_minus26;
    t_sint16 pic_init_qs_minus26;
    t_sint16 chroma_qp_index_offset;
    t_uint16 debl_flt_control_present_flag;
    t_uint16 constr_intra_pred_flag;
    t_uint16 redundant_pic_cnt_present_flag;
} t_pic_par;



/**
 * \brief Structure containing slice header information
 */
 
typedef struct
{
    t_uint16 first_mb_in_slice;
    t_uint16 slice_type;
    t_uint16 pic_par_set_id;
    t_uint16 frame_num;
    t_uint16 idr_pic_id;
    t_uint16 pic_order_cnt_lsb;
    t_sint32 delta_pic_order_cnt_bottom;
    t_sint32 delta_pic_order_cnt[2];
    t_uint16 redundant_pic_cnt;
    t_uint16 direct_spatial_mv_pred_flag;
    t_uint16 num_ref_idx_active_override_flag;
    t_uint16 num_ref_idx_l0_active_minus1;
    t_uint16 num_ref_idx_l1_active_minus1;
    t_uint16 cabac_init_idc;
    t_sint16 slice_qp_delta;
    t_uint16 sp_for_switch_flag;
    t_sint16 slice_qs_delta;
    t_uint16 disable_deblocking_filter_idc;
    t_sint16 slice_alpha_c0_offset_div2;
    t_sint16 slice_beta_offset_div2;
    t_uint16 slice_group_change_cycle;
    
    /* ref_pic_list_reordering() (list0 only) */
    t_uint16 ref_pic_list_reordering_flag_l0;
    t_uint16 reordering_of_pic_nums_idc[17];
    t_uint16 abs_diff_pic_num_minus1[16];
    t_uint16 long_term_pic_num[16];
       
    /* dec_ref_pic_marking() */
    t_uint16 no_output_of_prior_pics_flag;
    t_uint16 long_term_reference_flag;
    t_uint16 adaptive_ref_pic_marking_mode_flag;
    t_uint16 memory_management_control_operation[16];
    t_uint16 difference_of_pic_nums_minus1[16];
    t_uint16 marking_long_term_pic_num[16]; 
    t_uint16 long_term_frame_idx[16];
    t_uint16 max_long_term_frame_idx_plus1[16];   
    
    /* Extra parameters, not parsed */
    t_uint16 slice_num;
    t_sint16 slice_qp;
    t_uint16 error;
    t_uint16 error_type;
    t_uint16 nut;
    t_uint16 nri;
    t_uint16 mmco5_flag;
    t_uint16 len;
    
    t_seq_par *active_sp;
    t_pic_par *active_pp;
    
    /* list0 information for bS calculation (deblocking filter) */
    t_uint16 pic[16];
    
    /* Bitstream management */
    t_bit_buffer p_bsh_start;
    t_bit_buffer p_bsh_end;
    t_uint32 next;
    
} t_slice_hdr;



/**
 * \brief Structure containing all data needed by host.
 */
 
typedef struct 
{
    t_dec_buff *buff;
    t_bit_buffer *b;
    t_pic_par *pp; 
    t_seq_par *sp; 
    t_slice_hdr *sh; 
#if defined __ndk8500_a0__ 
    t_uint16 enable_hdtv;
#endif
} t_host_info;



/**
 * \brief Structure containing parsed command line arguments.
 */
 
typedef struct
{
    char *in_file;
	t_uint16 outmode;  
#if FULL_GUI == 1
    char *out_file;
    /* Post processing */
    t_uint16 pp_flag;
    
    t_uint16 bpp;
    t_uint16 acestr;
    t_uint16 c;
    t_uint16 b;
    t_uint16 mc1;
    t_uint16 mc2;
    t_uint16 mc3;                     
    t_uint16 mc4;
#elif FULL_GUI == 2
    char *out_file;                   
#endif
} t_host_args;

 
#endif

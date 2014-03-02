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

typedef t_uint16 * t_img_pel;
#include "frame.h"

/**
 * \brief Structure containing decoded pictures info
 */

typedef struct
{
    t_img_pel* ptr;
	  t_sint16 frame_pos;
    t_sint16 cup_ctx_idx;
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
#endif
} t_dec_pic_info;

typedef t_dec_pic_info t_dpb_info;


/**
 * \brief Structure containing buffers for CUP context for the whole DPB
 */

#define CUP_CTX_AVAIL_FIFO_SIZE  16+1+HAMAC_PIPE_SIZE+1

typedef struct
{
    t_uint8 *  cup_context[CUP_CTX_AVAIL_FIFO_SIZE];              /* the complete list of buffers                */
    t_sint16   cup_ctx_avail_fifo[CUP_CTX_AVAIL_FIFO_SIZE];       /* FIFO containing indexes of free CUP buffers */   
    t_uint16   cup_ctx_avail_fifo_r;                              /* read ptr to that FIFO                       */
    t_uint16   cup_ctx_avail_fifo_w;                              /* write ptr to that FIFO                      */
} t_cup_context_descriptor;



/**
 * \brief Structure containing buffers and relatives info
 */

typedef struct 
{
    /* Buffer for block infos */
#ifdef NO_HAMAC    
    t_block_info *b_info;
#else
    t_macroblock_info *b_info;
#endif

    /* Array for MB to Slice Group Map */
    t_uint16 *mb_slice_map;

    /* Frame buffers = initial allocated memory */
    t_frames *frames;
    t_uint16 max_nb_frames;

    /* Auxiliary buffer for deblocking filter */
    t_uint16 *aux_frame;
    t_uint16 *aux_frame2;

#ifdef __ndk5500_a0__
#define CUP_CONTEXT_SIZE_IN_BYTES 240
#else
#define CUP_CONTEXT_SIZE_IN_BYTES 192    
#endif

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    t_cup_context_descriptor cup_ctx_desc;
    t_uint8 * local_recon_buff;
#endif    
    t_uint8 * bits_intermediate_buffer;

    /* Array for deblocking filter parameter */
    tps_h4d_param *p_deblocking_paramv;

#ifdef __ndk5500_a0__
    t_uint8* external_sw; 
    t_uint8* external_sw2; 
#endif

    /* Pointer to current frame buffer */
    t_uint16 **curr_frame;
    t_dec_pic_info *curr_info;

    /* Array for reference list 0 */
    /* MC: added +1 because in reordering process the list is temporarily
       made one element longer (see std 8.2.4.3.1) */
    t_uint16 **list0[MAXNUMFRM+1];

    /* Array for reference list 1 */
    t_uint16 **list1[MAXNUMFRM+1];

    /* Variables for DPB management */
    t_uint16 DPBsize;
// +CR324558 CHANGE START FOR
	t_uint16 curDPBSize;
// -CR324558 CHANGE END OF
    t_uint16 AllocatedDPBsize;
    t_uint16 DPBfullness;
    t_sint16 MaxLongTermFrameIdx;
    t_uint16 numShortRef;
    t_uint16 numLongRef;
    t_sint32 PreviousFrameNum;
    t_uint16 initialized;

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
    t_uint16  HED_used; /* use of CABAC preprocessor or not for current task */
    t_uint16  curr_3d_format; /* 3D format associated to current frame being decoded */ 

    t_bit_buffer * keep_pb; 

    t_uint16 g_seek_mode; /* added by MC for SetPosition */
    t_uint32 g_decoded_frm_ctr;
	t_uint8 memoryAllocationFailed;
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


typedef struct {
  /* NZ: 6 for 4x4 (fixed) 
         2 for 8x8 !! This is the worst case in YUV 420 both for seq_scaling_list and pic_scaling_list */
  t_sint16 ScalingList4x4[6][16]; 
  t_sint16 ScalingList8x8[2][64];
  t_uint16 UseDefaultScalingMatrix4x4Flag[6];
  t_uint16 UseDefaultScalingMatrix8x8Flag[2];
} ts_ScalingList, *tps_ScalingList;


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

    /* Added for MAIN/HIGH profile extension */
    /* scaling matrixes */
    t_uint16 seq_scaling_matrix_present_flag;
    t_uint16 seq_scaling_list_present_flag[8];
    /* GG: every SPS may have its scaling lists (see Allegro streams) */
    ts_ScalingList seq_scaling_list;
} t_seq_par; /* 560 bytes */


typedef struct
{
  t_seq_par sp;

  t_uint16 bit_equal_to_one;
  t_uint16 num_views_minus1;
  t_uint16 view_id[MAX_NUM_VIEWS];
  t_uint16 num_anchor_refs_l0[MAX_NUM_VIEWS];
  t_uint16 anchor_ref_l0[MAX_NUM_VIEWS][15];
  t_uint16 num_anchor_refs_l1[MAX_NUM_VIEWS];
  t_uint16 anchor_ref_l1[MAX_NUM_VIEWS][15];

  t_uint16 num_non_anchor_refs_l0[MAX_NUM_VIEWS];
  t_uint16 non_anchor_ref_l0[MAX_NUM_VIEWS][15];
  t_uint16 num_non_anchor_refs_l1[MAX_NUM_VIEWS];
  t_uint16 non_anchor_ref_l1[MAX_NUM_VIEWS][15];
   
  t_uint16 num_level_values_signalled_minus1;
  t_uint16 level_idc[MAX_LEVELS_SIGNALLED];
  t_uint16 num_applicable_ops_minus1[MAX_LEVELS_SIGNALLED];
  t_uint16 applicable_op_temporal_id[MAX_LEVELS_SIGNALLED][MAX_OPERATION_POINTS];
  t_uint16 applicable_op_num_target_views_minus1[MAX_LEVELS_SIGNALLED][MAX_OPERATION_POINTS];
  t_uint16 applicable_op_target_view_id[MAX_LEVELS_SIGNALLED][MAX_OPERATION_POINTS][MAX_NUM_VIEWS];
  t_uint16 applicable_op_num_views_minus1[MAX_LEVELS_SIGNALLED][MAX_OPERATION_POINTS];

  t_uint16 mvc_vui_parameters_present_flag;
#if ENABLE_VUI == 1
  t_mvc_vui_data mvc_vui_parameters;
#endif
} t_subset_seq_par; 

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
    
    /* NZ: main/high profile */
    t_uint16 transform_8x8_mode_flag;
    t_uint16 pic_scaling_matrix_present_flag;
    t_uint16 pic_scaling_list_present_flag [6+(2*1)]; /* NZ: Worst Case in YUV 420*/
    /* GG: every PPS may have its scaling lists (see Allegro streams) */
    ts_ScalingList pic_scaling_list;
    t_sint16 second_chroma_qp_index_offset;
} t_pic_par;


/* added for MVC */
typedef struct nalunitheadermvcext_tag
{
   t_uint16 non_idr_flag;
   t_uint16 priority_id;
   t_uint16 view_id;
   t_uint16 temporal_id;
   t_uint16 anchor_pic_flag;
   t_uint16 inter_view_flag;
   t_uint16 reserved_one_bit;
   t_uint16 iPrefixNALU;
} NALUnitHeaderMVCExt_t;


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
    t_uint16 abs_diff_pic_num_minus1[16+1];
    t_uint16 long_term_pic_num[16+1];     /* MC: added +1 because in reordering process 
                                              the list is temporarily
                                              made one element longer (see std 8.2.4.3.1) */
       
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
    /* list1 information for bS calculation (deblocking filter) */
    t_uint16 pic_l1[16];
    
    /* Bitstream management */
    t_bit_buffer p_bsh_start;
    t_bit_buffer p_bsh_end;
    t_uint32 next;
    t_uint32 aeb_counter; /* counts the anti-emulation bytes in slice header */
    
    /* Added for MAIN/HIGH profile extension */
    t_uint16 ref_pic_list_reordering_flag_l1;
    t_uint16 reordering_of_pic_nums_idc_l1[17];
    t_uint16 abs_diff_pic_num_minus1_l1[17];
    t_uint16 long_term_pic_num_l1[17];

    /* added for MVC */
    t_uint16 abs_diff_view_idx_minus1_l1[16];
    t_uint16 view_id;
    t_uint16 inter_view_flag;
    t_uint16 anchor_pic_flag;
    t_sint16 svc_extension_flag;
    t_uint16 abs_diff_view_idx_minus1[16];
    NALUnitHeaderMVCExt_t NaluHeaderMVCExt;
    
    /* weighted prediction */
    t_uint16 luma_log2_weight_denom;
    t_uint16 chroma_log2_weight_denom;
    t_sint16 luma_weight_l0[16];
    t_sint16 luma_offset_l0[16];
    t_sint16 luma_weight_l1[16];
    t_sint16 luma_offset_l1[16];
    t_sint16 chroma_weight_l0[16][2];
    t_sint16 chroma_offset_l0[16][2];
    t_sint16 chroma_weight_l1[16][2];
    t_sint16 chroma_offset_l1[16][2];
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
    t_uint32 aeb_counter; /* to count anti-emulation bytes */
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

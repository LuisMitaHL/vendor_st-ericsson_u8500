/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated t1xhv_vdc_h264.idt defined type */
#if !defined(_t1xhv_vdc_h264_idt)
#define _t1xhv_vdc_h264_idt

#include <host/t1xhv_vdc.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_vdc_h264_param_in {
  t_ushort_value log2_max_frame_num;
  t_ushort_value pic_order_cnt_type;
  t_ushort_value log2_max_pic_order_cnt_lsb;
  t_ushort_value delta_pic_order_always_zero_flag;
  t_ushort_value pic_width_in_mbs;
  t_ushort_value pic_height_in_mbs;
  t_ushort_value frame_mbs_only_flag;
  t_ushort_value mb_adaptive_frame_field_flag;
  t_ushort_value direct_8x8_inference_flag;
  t_ushort_value monochrome;
  t_ushort_value DPBSize;
  t_ushort_value curr_pic_idx;
  t_ahb_address dpb_addresses[17];
  t_ahb_address addr_cup_ctx[17];
  t_long_value poc_list[17];
  t_ushort_value entropy_coding_mode_flag;
  t_ushort_value pic_order_present_flag;
  t_ushort_value num_ref_idx_l0_active_minus1;
  t_ushort_value num_ref_idx_l1_active_minus1;
  t_ushort_value weighted_pred_flag;
  t_ushort_value weighted_bipred_idc;
  t_ushort_value pic_init_qp;
  t_short_value chroma_qp_index_offset;
  t_ushort_value deblocking_filter_control_present_flag;
  t_ushort_value constr_intra_pred_flag;
  t_ushort_value transform_8x8_mode_flag;
  t_short_value second_chroma_qp_index_offset;
  t_ushort_value DBLK_flag;
  t_ushort_value intra_conc;
  t_ushort_value HED_used;
  t_ushort_value reserved16_0;
  t_ahb_address addr_scaling_matrix;
  t_ahb_address addr_first_slice;
  t_ulong_value reserved32_0;
} ts_t1xhv_vdc_h264_param_in;

typedef ts_t1xhv_vdc_h264_param_in* tps_t1xhv_vdc_h264_param_in;

typedef struct t_xyuv_ts_t1xhv_vdc_h264_param_inout {
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;
  t_ulong_value reserved_3;
  t_ulong_value reserved_4;
} ts_t1xhv_vdc_h264_param_inout;

typedef ts_t1xhv_vdc_h264_param_inout* tps_t1xhv_vdc_h264_param_inout;

typedef struct t_xyuv_ts_t1xhv_vdc_h264_param_out {
  t_ushort_value picture_loss;
  t_ushort_value mb_count;
  t_ushort_value mb_intra;
  t_ushort_value reserved_2;
  t_ulong_value reserved_3;
  t_ulong_value reserved_4;
  t_ushort_value slice_loss_first_mb[8];
  t_ushort_value slice_loss_mb_num[8];
} ts_t1xhv_vdc_h264_param_out;

typedef ts_t1xhv_vdc_h264_param_out* tps_t1xhv_vdc_h264_param_out;

typedef struct t_xyuv_ts_t1xhv_vdc_h264_slice {
  t_ushort_value discarded_slice;
  t_ushort_value first_mb_in_slice;
  t_ushort_value slice_type;
  t_ushort_value slice_num;
  t_ushort_value direct_spatial_mv_pred_flag;
  t_ushort_value num_ref_idx_l0_active_minus1;
  t_ushort_value num_ref_idx_l1_active_minus1;
  t_ushort_value slice_qp;
  t_ushort_value s_info_disable_filter;
  t_ushort_value s_info_alpha_c0_offset_div2;
  t_ushort_value s_info_beta_offset_div2;
  t_ushort_value slice_header_bit_offset;
  t_ushort_value listX_RefIdx2RefPic[16];
  t_ulong_value long_termlist;
  t_ahb_address addr_weighted_pred_buffer;
  t_ahb_address addr_bitstream_buf_struct;
  t_ahb_address addr_bitstream_start;
  t_ulong_value bitstream_offset;
  t_ulong_value bitstream_size_in_bytes;
  t_ahb_address addr_next_h264_slice;
  t_ulong_value reserved32_0;
  t_ulong_value reserved32_1;
  t_ulong_value reserved32_2;
} ts_t1xhv_vdc_h264_slice;

typedef ts_t1xhv_vdc_h264_slice* tps_t1xhv_vdc_h264_slice;

typedef struct t_xyuv_ts_t1xhv_vdc_h264_scaling_list {
  t_ushort_value scaling_matrix_4x4[6][16];
  t_ushort_value scaling_matrix_8x8[2][64];
} ts_t1xhv_vdc_h264_scaling_list;

typedef ts_t1xhv_vdc_h264_scaling_list* tps_t1xhv_vdc_h264_scaling_list;

typedef struct t_xyuv_ts_t1xhv_vdc_h264_weight_list {
  t_ushort_value luma_log2_weight_denom;
  t_ushort_value chroma_log2_weight_denom;
  t_ulong_value reserved32_0;
  t_ulong_value reserved32_1;
  t_ulong_value reserved32_2;
  t_short_value weight_l0[16][3];
  t_short_value offset_l0[16][3];
  t_short_value weight_l1[16][3];
  t_short_value offset_l1[16][3];
} ts_t1xhv_vdc_h264_weight_list;

typedef ts_t1xhv_vdc_h264_weight_list* tps_t1xhv_vdc_h264_weight_list;

#endif

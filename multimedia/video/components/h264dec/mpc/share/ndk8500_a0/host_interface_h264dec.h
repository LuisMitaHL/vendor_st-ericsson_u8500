/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _T1XHV_HOST_INTERFACE_H264DEC_H_
#define _T1XHV_HOST_INTERFACE_H264DEC_H_

#include "t1xhv_retarget.h"

/** \brief Structure for parameters FROM Host for a H264 decode task */
typedef struct t1xhv_vdc_h264_param_in {
    /* SPS */
    t_ushort_value  log2_max_frame_num;               /**<\brief log2_max_frame_num */
    t_ushort_value  pic_order_cnt_type;
    t_ushort_value  log2_max_pic_order_cnt_lsb;       /**<\brief log2_max_pic_order_cnt_lsb */
    t_ushort_value  delta_pic_order_always_zero_flag;
    t_ushort_value  pic_width_in_mbs;                 /* Seq */ /**<\brief pic width in macroblocks */ 
    t_ushort_value  pic_height_in_mbs;          /* Seq */ /**<\brief pic height in macroblocks */
    t_ushort_value  frame_mbs_only_flag;              /**<\brief frame_mbs_only_flag */
    t_ushort_value  mb_adaptive_frame_field_flag;
    t_ushort_value  direct_8x8_inference_flag;	      /* SEQ */ /**<\brief direct_8x8_inference_flag */
    t_ushort_value  monochrome;
    t_ushort_value  DPBSize;                          /**<\brief DPBSize*/
    t_ushort_value  curr_pic_idx;
    t_ahb_address   dpb_addresses[17];
    t_ahb_address   addr_cup_ctx[17];
    t_long_value    poc_list[17];
    /* PPS */
    t_ushort_value  entropy_coding_mode_flag;		      /* PIC */ /**<\brief entropy_coding_mode_flag */
    t_ushort_value  pic_order_present_flag;
    t_ushort_value  num_ref_idx_l0_active_minus1;
    t_ushort_value  num_ref_idx_l1_active_minus1;
    t_ushort_value  weighted_pred_flag;
    t_ushort_value  weighted_bipred_idc;
    t_ushort_value  pic_init_qp;		                  /* PIC */ /**<\brief pic_init_qp_minus26 + 26 */
    t_short_value   chroma_qp_index_offset;
    t_ushort_value  deblocking_filter_control_present_flag;
    t_ushort_value  constr_intra_pred_flag;
    t_ushort_value  transform_8x8_mode_flag;		      /* ? PIC */ /**<\brief transform_8x8_mode_flag */
    t_short_value   second_chroma_qp_index_offset;
    t_ushort_value  DBLK_flag;                        /**<\brief DBLK_flag == 0 : No deblocking (no more bit true )   *
                                                        *        DBLK_flag == 1 : Deblocking at the end of decode     *
                                                        *        DBLK_flag == 3 : Deblocking parallelized with decode *
                                                        *                         ( possible when no FMO )            */
    t_ushort_value  intra_conc;                       /**<\brief flag for concealment updated in SVA                  */    
    t_ushort_value  HED_used;                       /**<\brief reserved                  */    
    t_ushort_value  reserved16_0;                   /**<\brief reserved */
    t_ahb_address   addr_scaling_matrix;              /**<\brief address of scaling matrix (Null if no scaling matrix) */
    t_ahb_address   addr_first_slice;            /**<\brief address of first slice info structure                */
    t_long_value    reserved32_0;
} ts_t1xhv_vdc_h264_param_in, *tps_t1xhv_vdc_h264_param_in; /* 18 x (4 x 32bits) */


/** \brief Structure for parameters FROM and TO Host for a H264 decode task */
typedef struct t1xhv_vdc_h264_param_inout {

    t_ulong_value  reserved_1;                  /**<\brief Reserved 32                            */
    t_ulong_value  reserved_2;                  /**<\brief Reserved 32                            */
    t_ulong_value  reserved_3;                  /**<\brief Reserved 32                            */
    t_ulong_value  reserved_4;                  /**<\brief Reserved 32                            */

} ts_t1xhv_vdc_h264_param_inout, *tps_t1xhv_vdc_h264_param_inout;


/** \brief Structure for parameters TO Host for a H264 decode task */
typedef struct t1xhv_vdc_h264_param_out {

    t_ushort_value picture_loss;                /**<\brief number of decoded macroblocks          */
    t_ushort_value mb_count;                    /**<\brief number of decoded macroblocks          */
    t_ushort_value mb_intra;                    /**<\brief number of intra MB in P slices         */
    t_ushort_value reserved_2;                  /**<\brief reserved 16                           */
    t_ulong_value  reserved_3;                  /**<\brief Reserved 32                            */
    t_ulong_value  reserved_4;                  /**<\brief Reserved 32                            */
    t_ushort_value slice_loss_first_mb[8];
    t_ushort_value slice_loss_mb_num[8];

} ts_t1xhv_vdc_h264_param_out, *tps_t1xhv_vdc_h264_param_out;


/** \brief Structure for parameters TO Host for H264 to decode 1 slice */
typedef struct t1xhv_vdc_h264_slice {
    t_ushort_value  discarded_slice;                  /**<\brief discarded_slice (not decoded slice)              */
    t_ushort_value  first_mb_in_slice;
    t_ushort_value  slice_type;                       /**<\brief slice coding type                    */
    t_ushort_value  slice_num;                        /**<\brief number of the slice in the current frame         */
    t_ushort_value  direct_spatial_mv_pred_flag;      /**<\brief indicate if spatial pred is used for direct pred  */
    t_ushort_value  num_ref_idx_l0_active_minus1;     /**<\brief maximum reference index for reference frame list0 */
    t_ushort_value  num_ref_idx_l1_active_minus1;     /**<\brief maximum reference index for reference frame list1 */
    t_ushort_value  slice_qp;                         /**<\brief Initial Qp value for the slice : pic_init_qp + slice_qp_delta */
    t_ushort_value  s_info_disable_filter;            /**<\brief 1 = disable filter for the slie                  */
    t_ushort_value  s_info_alpha_c0_offset_div2;      /**<\brief offset used for alpha and tc0 tables (deblocking)*/
    t_ushort_value  s_info_beta_offset_div2;          /**<\brief offset used for beta table (deblocking)          */
    t_ushort_value  slice_header_bit_offset;
    t_ushort_value  listX_RefIdx2RefPic[16];
    t_ulong_value   long_termlist;
    t_ahb_address   addr_weighted_pred_buffer;        /**<\brief address for buffer for weighted prediction */
    t_ahb_address   addr_bitstream_buf_struct;        /**<\brief bitstream buffer structure address (WO HED)  */
    t_ahb_address   addr_bitstream_start;             /**<\brief bitstream position 16 byte aligned (WO HED)  */
    t_ulong_value   bitstream_offset;                 /**<\brief bitstream position offset in bits (With HED : offset to reach MB header */
    t_ulong_value   bitstream_size_in_bytes;          /**<\brief bitstream size in bytes for current slice (WO HED)        */
    t_ahb_address   addr_next_h264_slice;             /**<\brief address for next slice (NULL is no more slice)   */
    t_ulong_value   reserved32_0;            /**<\brief reserved 32bits                */
    t_ulong_value   reserved32_1;            /**<\brief reserved 32bits                */
    t_ulong_value   reserved32_2;            /**<\brief reserved 32bits                */
} ts_t1xhv_vdc_h264_slice, *tps_t1xhv_vdc_h264_slice; /* 6 x (4 x 32bits) */


/** \brief Structure for parameters FROM Host for a H264 decode scaling lists slice? level */
typedef struct t1xhv_vdc_h264_scaling_list {
    t_ushort_value  scaling_matrix_4x4[6][16];        /**<\brief scaling matrix for 4x4 pred                          */
    t_ushort_value  scaling_matrix_8x8[2][64];        /**<\brief scaling matrix for 8x8 pred                          */
} ts_t1xhv_vdc_h264_scaling_list, *tps_t1xhv_vdc_h264_scaling_list;


/** \brief Structure for parameters FROM Host for a H264 decode weighted lists slice level */
typedef struct t1xhv_vdc_h264_weight_list {
    t_ushort_value luma_log2_weight_denom;
    t_ushort_value chroma_log2_weight_denom;
    t_ulong_value  reserved32_0;
    t_ulong_value  reserved32_1;
    t_ulong_value  reserved32_2;
    t_short_value  weight_l0[16][3];
    t_short_value  offset_l0[16][3];
    t_short_value  weight_l1[16][3];
    t_short_value  offset_l1[16][3];           
} ts_t1xhv_vdc_h264_weight_list, *tps_t1xhv_vdc_h264_weight_list;


#endif /* _T1XHV_HOST_INTERFACE_H264DEC_H_ */

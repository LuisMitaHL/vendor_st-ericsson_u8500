/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
 * Structure used for Host Interface
 * Are defined : 
 * \arg parameters for tasks
 * \arg parameters for buffer
 * Types must be defined in t1xhv_retarget.h with following convention
 * \arg t_ulong_value  = 32 bit unsigned integer (Little endian for 16 bit word)
 * \arg t_long_value   = 32 bit signed integer (Little endian for 16 bit word)
 * \arg t_ushort_value = 16 bit unsigned integer
 * \arg t_short_value  = 16 bit signed integer
 * \arg t_ahb_address  = 32 bit unsigned integer to define AHB address
 * \arg t_time stamp   = 32 bit unsigned integer to define a time stamp
 */
/*****************************************************************************/

#ifndef _T1XHV_HOST_INTERFACE_H_
#define _T1XHV_HOST_INTERFACE_H_

/*
 * Includes       
 */

#ifdef __T1XHV_NMF_ENV

/* Avoid conflicts with codecs that have not yet splitten their interface */
#ifndef _T1XHV_HOST_INTERFACE_COMMON_H_

#include "inc/type.h"
#include <t1xhv_common.idt>         /* Include common data structure  */
// #include <t1xhv_vdc_mpeg4.idt>      /* And MPEG4 decoder specific ones. */
// #include <t1xhv_vec_mpeg4.idt>      /* And MPEG4 encoder specific ones. */
// #include <t1xhv_vec_h264.idt>       /* And H264 encoder specific ones. */
// #include <t1xhv_vec_jpeg.idt>       /* And JPEGenecoder specific ones. */
                                    /* and so on for all others codecs. */

#endif /* _T1XHV_HOST_INTERFACE_COMMON_H_ */

#else /* __T1XHV_NMF_ENV */

/* Avoid conflicts with codecs that have not yet splitten their interface */
#ifndef _T1XHV_HOST_INTERFACE_COMMON_H_

#include "t1xhv_retarget.h"

/*
 * Types
 */

/*****************************************************************************/
/**
 * \brief  Parameter common structure
 * \author Philippe Rochette
 *
 * Parameter common structure
 */
/*****************************************************************************/

/** 
 * \brief This structure define one link to a subtask description. 
 */
typedef struct t1xhv_subtask_link {

    t_ahb_address addr;           /**<\brief Add. of associated subtask */
    t_ulong_value type;           /**<\brief Define type for subtask */
    t_time_stamp  execution_time_stamp; /**<\brief Define time to start subtask */
    t_ulong_value dependency;     /**<\brief Coded depend. between tasks */

} ts_t1xhv_subtask_link, *tps_t1xhv_subtask_link;


/** 
 * \brief This structure define Parameters in Memory for Subtask parameters. 
 */
typedef struct t1xhv_subtask_descriptor {

    ts_t1xhv_subtask_link s_next_subtask;      /**<\brief Link to next subtask 
                                                * -- ts_t1xhv_subtask_link      */
    ts_t1xhv_subtask_link s_current_subtask;   /**<\brief Link to current subtask  
                                                * -- ts_t1xhv_subtask_link      */
    ts_t1xhv_subtask_link s_interrupt_subtask; /**<\brief Link to interrupt subtask  
                                                * -- ts_t1xhv_subtask_link      */
    t_ulong_value         task_count;          /**<\brief Task counter (0 if no 
                                                *          more task) 
                                                */

} ts_t1xhv_subtask_descriptor, *tps_t1xhv_subtask_descriptor;


/** \brief This structure define Parameters needed to For begin/end of buffer. */
typedef struct t1xhv_bitstream_buf_pos {

    t_ahb_address   addr_bitstream_buf_struct; /**<\brief Choose buffer structure            */
    t_ahb_address   addr_bitstream_start;      /**<\brief Bitstream Start add. inside buffer */
    t_ulong_value   bitstream_offset;          /**<\brief Bitstream offset in bits           */
    t_ulong_value   reserved_1;                /**<\brief Reserved 32                        */

} ts_t1xhv_bitstream_buf_pos, *tps_t1xhv_bitstream_buf_pos;


/** \brief This structure define a bitstream buffer. */
typedef struct t1xhv_bitstream_buf {

    t_ahb_address addr_buffer_start; /**<\brief Buffer start                 */
    t_ahb_address addr_buffer_end;   /**<\brief Buffer end                   */
    t_ahb_address addr_window_start; /**<\brief Window start (inside buffer) */
    t_ahb_address addr_window_end;   /**<\brief Windows end  (inside buffer) */

} ts_t1xhv_bitstream_buf, *tps_t1xhv_bitstream_buf;


/** \brief This structure define a link buffer. */
typedef struct t1xhv_bitstream_buf_link {

    t_ahb_address addr_next_buf_link;   /**<\brief Address next structure */
    t_ahb_address addr_prev_buf_link;   /**<\brief Address prev structure */
    t_ahb_address addr_buffer_start;    /**<\brief Bitstream buffer start */
    t_ahb_address addr_buffer_end;      /**<\brief Bitstream buffer end   */

} ts_t1xhv_bitstream_buf_link, *tps_t1xhv_bitstream_buf_link;


/** \brief This structure define an header buffer. */
typedef struct t1xhv_header_buf {

    t_ahb_address  addr_header_buffer;  /**<\brief Start add. of the header buffer */
    t_ulong_value  header_size;         /**<\brief Header size                     */
    t_ahb_address  reserved_1;          /**<\brief Reserved 32                     */
    t_ahb_address  reserved_2;          /**<\brief Reserved 32                     */

} ts_t1xhv_header_buf, *tps_t1xhv_header_buf;




/*****************************************************************************/
/**
 * \brief  Parameter structure decode
 * \author Philippe Rochette
 *
 * Parameter structure for decode H264, MPEG4, JPEG and H263. 
 * Hamac Video Spec v0.1 sections 6.5 and 13.2
 */
/*****************************************************************************/

/** \brief This structure define description of a subtask decode. */
typedef struct t1xhv_vdc_subtask_param {

    ts_t1xhv_subtask_link s_link;                        /**<\brief Link to next subtask (chained list) 
                                                          *             -- ts_t1xhv_subtask_link
                                                          */
    t_ahb_address         addr_in_frame_buffer;          /**<\brief Add. of struct for input frame buffer
                                                          *             -- ts_t1xhv_vdc_frame_buffer_in
                                                          */
    t_ahb_address         addr_out_frame_buffer;         /**<\brief Add. of struct for output frame buffer 
                                                          *             -- ts_t1xhv_vdc_frame_buffer_out
                                                          */
    t_ahb_address         addr_internal_buffer;          /**<\brief  Add. of struct for internal buffer 
                                                          *              -- ts_t1xhv_vdc_internal_buf
                                                          */
    t_ahb_address         addr_in_bitstream_buffer;      /**<\brief Add. of struct for in bitstream buffer 
                                                          *         -- ts_t1xhv_bitstream_buffer
                                                          */
    t_ahb_address         addr_out_bitstream_buffer;     /**<\brief Add. of struct for out bitstream buffer
                                                          *          -- ts_t1xhv_bitstream_buffer_position
                                                          */
    t_ahb_address         addr_in_parameters;            /**<\brief Add. of struct for input parameters 
                                                          *             of decode (depend on standard)
                                                          */
    t_ahb_address         addr_out_parameters;           /**<\brief Add. of struct for output parameters
                                                          *             of decode (depend on standard)
                                                          */
    t_ahb_address         addr_in_frame_parameters;      /**<\brief Add. of struct for input frame parameters 
                                                          *             of decode (depend on standard)
                                                          */
    t_ahb_address         addr_out_frame_parameters;     /**<\brief Add. of struct for output frame parameters 
                                                          *             of decode (depend on standard)
                                                          */
    t_ahb_address         addr_vpp_dummy;               /**<\brief Reserved 32                          */
    t_ahb_address         addr_dma_cup_context;                    /**<\brief Reserved 32                          */
    t_ahb_address         reserved_3;                    /**<\brief Reserved 32                          */

} ts_t1xhv_vdc_subtask_param, *tps_t1xhv_vdc_subtask_param;



/** \brief This structure define a reference frame buffer. */
typedef struct t1xhv_vdc_frame_buf_in {

    t_ahb_address   addr_fwd_ref_buffer;  /**<\brief Address of Forward reference buffer. */
    t_ahb_address   addr_bwd_ref_buffer; /**<\brief Address of backward reference buffer */
    t_ulong_value   reserved_2;           /**<\brief Reserved 32 */
    t_ulong_value   reserved_3;           /**<\brief Reserved 32                          */

} ts_t1xhv_vdc_frame_buf_in, *tps_t1xhv_vdc_frame_buf_in;


/** \brief This structure define a reference frame buffer. */
typedef struct t1xhv_vdc_internal_buf {

    t_ahb_address   addr_h264d_H4D_buffer;     /**<\brief Address of temporary buffer used by H4D for H264. */
    t_ahb_address   addr_h264d_local_reconstructed;     /**<\brief Address of block_info                             */
    t_ahb_address   addr_h264d_mb_slice_map;   /**<\brief Address of mb_slice_map                           */
    t_ahb_address   addr_mv_history_buffer;                /**<\brief VC1 motion vector history buffer (for B framesdecoding)    */

} ts_t1xhv_vdc_internal_buf, *tps_t1xhv_vdc_internal_buf;


/** \brief This structure define an output frame buffer. */
typedef struct t1xhv_vdc_frame_buf_out {

    t_ahb_address   addr_dest_buffer;             /**<\brief Address of output frame buffer.   */
    t_ahb_address   addr_deblocking_param_buffer; /**<\brief Address of parameters for PPP.    */
    t_ahb_address   addr_motion_vector_buffer;    /**<\brief Start add of motion vector buffer */
    t_ahb_address   addr_jpeg_coef_buffer;        /**<\brief Start address of JPEG Coef buffer */
    t_ahb_address   addr_jpeg_line_buffer;        /**<\brief Start address of JPEG line buffer */
    t_ulong_value   reserved_1;                   /**<\brief Reserved 32                       */
    t_ulong_value   reserved_2;                   /**<\brief Reserved 32                       */
    t_ulong_value   reserved_3;                   /**<\brief Reserved 32                       */

} ts_t1xhv_vdc_frame_buf_out, *tps_t1xhv_vdc_frame_buf_out;

#endif /* _T1XHV_HOST_INTERFACE_COMMON_H_ */


/** \brief Structure for parameters FROM Host for a H264 decode task */
typedef struct t1xhv_vdc_h264_param_in {
    /* SPS */
    t_ushort_value  log2_max_frame_num;               /**<\brief log2_max_frame_num */
    t_ushort_value  pic_order_cnt_type;
    t_ushort_value  log2_max_pic_order_cnt_lsb;       /**<\brief log2_max_pic_order_cnt_lsb */
    t_ushort_value  delta_pic_order_always_zero_flag;
    t_ushort_value  pic_width_in_mbs;                 /* Seq */ /**<\brief pic width in macroblocks */ 
    t_ushort_value  pic_height_in_map_units;          /* Seq */ /**<\brief pic height in macroblocks */
    t_ushort_value  frame_mbs_only_flag;              /**<\brief frame_mbs_only_flag */
    t_ushort_value  mb_adaptive_frame_field_flag;
    t_ushort_value  direct_8x8_inference_flag;	      /* SEQ */ /**<\brief direct_8x8_inference_flag */
    t_ushort_value  monochrome;
    t_ushort_value  DPBSize;                          /**<\brief DPBSize*/
    t_ushort_value  curr_pic_idx;
    t_long_value    currPicOrField;
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
    t_ushort_value  FMO_ON_flag;                      /**<\brief  FMO_flag == 1 : bitstream is FMO                    *
                                                            FMO_flag == 0 : bitstream is raster scan            */
    t_ushort_value  DBLK_flag;                        /**<\brief DBLK_flag == 0 : No deblocking (no more bit true )   *
                                                        *        DBLK_flag == 1 : Deblocking at the end of decode     *
                                                        *        DBLK_flag == 3 : Deblocking parallelized with decode *
                                                        *                         ( possible when no FMO )            */
    t_ushort_value  ERC_used;                         /**<\brief ERC_used == 0 : no Errors in bitstream can occur     *
                                                        *        ERC_used == 1 : Errors in bitstream can occur        */
    t_ushort_value  intra_conc;                       /**<\brief flag for concealment updated in SVA                  */    
    t_ushort_value  HED_used;                       /**<\brief reserved                  */    
    t_ushort_value  reserved16_0;                   /**<\brief reserved */
    t_ahb_address   addr_scaling_matrix;              /**<\brief address of scaling matrix (Null if no scaling matrix) */
    t_ahb_address   addr_first_slice;            /**<\brief address of first slice info structure                */
    t_ulong_value   reserved32_0;            /**<\brief reserved 32bits                */
    t_ulong_value   reserved32_1;            /**<\brief reserved 32bits                */
    t_ulong_value   reserved32_2;            /**<\brief reserved 32bits                */
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
    t_ulong_value  reserved_2;                  /**<\brief reserved 32                            */
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



/** \brief Structure for parameters FROM Host for a MPEG4 decode task */
typedef struct t1xhv_vdc_mpeg4_param_in {
  t_ushort_value picture_coding_type;           /**<\brief Current pict I,P or B       */
    t_ushort_value quant;                         /**<\brief Quantization parameter      */
    t_ushort_value quant_type                   ; /**<\brief ASP-Selects method 1 (1) or 2 (0) inverse quantisation */
    t_ushort_value intra_quant_mat[64]          ; /**<\brief ASP-inverse intra quantisation matrix                  */
    t_ushort_value nonintra_quant_mat[64]       ; /**<\brief ASP-inverse non intra quantisation matrix              */
    t_ushort_value low_delay                    ; /**<\brief ASP-if 0 => B frames        */
    t_ushort_value interlaced                   ; /**<\brief ASP-if 1 => interlaced mode */
    t_ushort_value rounding_type;                 /**<\brief Rounding type               */
    t_ushort_value intra_dc_vlc_thr;              /**<\brief Threshold to consider DC as AC coeff   */
    t_ushort_value vop_fcode_forward;             /**<\brief Fcode to decode MV          */
    t_ushort_value vop_fcode_backward           ; /**<\brief ASP-Fcode to decode MV      */
    t_ushort_value frame_width;                   /**<\brief Nb of pixel per line        */
    t_ushort_value frame_height;                  /**<\brief Nb of line                  */
    t_ushort_value flag_short_header;             /**<\brief Short Header mode if =1     */
    t_ushort_value modulo_time_base             ; /**<\brief ASP-needed for TRB and TRD computation */
    t_ushort_value vop_time_increment           ; /**<\brief ASP-needed for TRB and TRD computation */
    t_ushort_value vop_time_increment_resolution; /**<\brief VOP time increment          */
    t_ushort_value resync_marker_disable;         /**<\brief Resync Marker Disable       */
    t_ushort_value data_partitioned;              /**<\brief Data Partitioned            */
    t_ushort_value reversible_vlc;                /**<\brief Reversible VLC              */
    t_ushort_value error_concealment_config;      /**<\brief Error Concealment MPEG4     */
    t_ushort_value reserved_1;                    /**<\brief Reserved 16                 */
    t_ulong_value  reserved_2;                    /**<\brief Reserved 32                 */
  t_ulong_value reserved_3;

} ts_t1xhv_vdc_mpeg4_param_in, *tps_t1xhv_vdc_mpeg4_param_in;

/** \brief Structure for parameters FROM and TO Host for a MPEG4 decode task */
typedef struct t1xhv_vdc_mpeg4_param_inout {

    t_ulong_value  reserved_1;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_2;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_3;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_4;                     /**<\brief Reserved 32            */

} ts_t1xhv_vdc_mpeg4_param_inout, *tps_t1xhv_vdc_mpeg4_param_inout;


/** \brief Structure for parameters TO Host for a MPEG4 decode task */
typedef struct t1xhv_vdc_mpeg4_param_out {

    t_ushort_value error_type;             /**<\brief Return bitstream error type    */
    t_ushort_value picture_loss;           /**<\brief Picturee loss flags            */
    t_ushort_value slice_loss_first_mb[8]; /**<\brief Slice lost first macroblock    */
    t_ushort_value slice_loss_mb_num[8];   /**<\brief Slice loss MB number           */
    t_ushort_value concealed_mb_num;       /**<\brief NB of Concealed MacroBlock     */
    t_ushort_value concealed_vp_num;       /**<\brief NB of Concealed video packets  */
    t_ushort_value decoded_vp_num;         /**<\brief NB of video packets decoded    */

    t_ushort_value  reserved_1;             /**<\brief Reserved 32                    */    
    t_ulong_value  reserved_2;             /**<\brief Reserved 32                    */    

} ts_t1xhv_vdc_mpeg4_param_out, *tps_t1xhv_vdc_mpeg4_param_out;


/** \brief Structure for parameters FROM and TO Host for a MPEG2 decode task */
typedef struct t1xhv_vdc_mpeg2_param_in {
  /* not used t_ushort_value horizontal_size; */
    t_ushort_value vertical_size;
    t_ushort_value mb_width;
    t_ushort_value mb_height;
  /* not used t_ushort_value progressive_sequence; */
  /* not used t_ushort_value low_delay; */
    
    t_ushort_value intra_quantizer_matrix[64];
    t_ushort_value non_intra_quantizer_matrix[64];
    
  /* not used t_ulong_value  frame_rate; */
  /* not used t_ulong_value  bit_rate_value; */
     
  /* not used t_ulong_value  vbv_buffer_size; */
  /* not used t_ushort_value gop_flag; */
  /* not used t_ushort_value closed_gop; */
    
  /* not used t_ushort_value broken_link; */
  /* not used t_ushort_value temporal_reference; */
    t_ushort_value picture_coding_type;
  /* not used t_ushort_value vbv_delay; */
    
    t_ushort_value full_pel_forward_vector;
    t_ushort_value forward_f_code;
    t_ushort_value full_pel_backward_vector;
    t_ushort_value backward_f_code;
    
    t_ushort_value f_code[2][2];
    
    t_ushort_value intra_dc_precision;
    t_ushort_value picture_structure;
    t_ushort_value top_field_first;
    t_ushort_value frame_pred_frame_dct;
    
    t_ushort_value concealment_motion_vectors;
    t_ushort_value q_scale_type;
    t_ushort_value intra_vlc_format;
    t_ushort_value alternate_scan;
    
  /* not used t_ushort_value repeat_first_field; */
  /* not used t_ushort_value chroma_420_type; */
  /* not used t_ushort_value progressive_frame; */
    t_ushort_value scalable_mode;
    t_ushort_value MPEG2_Flag;
 t_ulong_value reserved1;

} ts_t1xhv_vdc_mpeg2_param_in, *tps_t1xhv_vdc_mpeg2_param_in;

/** \brief Structure for parameters FROM and TO Host for a MPEG2 decode task */
typedef struct t1xhv_vdc_mpeg2_param_inout {

    t_ulong_value  reserved_1;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_2;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_3;                     /**<\brief Reserved 32            */
    t_ulong_value  reserved_4;                     /**<\brief Reserved 32            */

} ts_t1xhv_vdc_mpeg2_param_inout, *tps_t1xhv_vdc_mpeg2_param_inout;

typedef struct t1xhv_vdc_mpeg2_param_out {
    t_ushort_value error_type;             /**<\brief Return bitstream error type    */
    t_ushort_value reserved_1;
    t_ulong_value  reserved_2;
    t_ulong_value  reserved_3;
    t_ulong_value  reserved_4;

} ts_t1xhv_vdc_mpeg2_param_out, *tps_t1xhv_vdc_mpeg2_param_out;


/** \brief Structure for parameters FROM Host for a H263 decode task */
typedef struct t1xhv_vdc_h263_param_in {

    t_ushort_value picture_coding_type;      /**<\brief True if inter picture, false if intra */
    t_ushort_value quant;                    /**<\brief Quantification parameter for current 
                                              *          frame 
                                              */
    t_ushort_value rounding_type;            /**<\brief Rounding control parameters*/ 
    t_ushort_value enable_annexes;           /**<\brief - Enable mv over picture boundary
                                              *         - Enable 4 mv
                                              *         - Enable AC/DC prediction
                                              *         - Enable deblocking filter
                                              *         - Enable slice structure
                                              *         - Enable modified quantization
                                              */
     t_ushort_value frame_width;              /**<\brief Nb of pixel per line    */  
    t_ushort_value frame_height;             /**<\brief Nb of line              */ 
    t_ushort_value error_concealment_config; /**<\brief Error Concealment MPEG4 */ 
    t_ushort_value reserved_1;               /**<\brief Start code detection    */

} ts_t1xhv_vdc_h263_param_in, *tps_t1xhv_vdc_h263_param_in;

/** \brief Structure for parameters FROM and TO Host for a H263 decode task */
typedef struct t1xhv_vdc_h263_param_inout {

    t_ulong_value  reserved_1;               /**<\brief Reserved 32             */
    t_ulong_value  reserved_2;               /**<\brief Reserved 32             */
    t_ulong_value  reserved_3;               /**<\brief Reserved 32             */
    t_ulong_value  reserved_4;               /**<\brief Reserved 32             */

} ts_t1xhv_vdc_h263_param_inout, *tps_t1xhv_vdc_h263_param_inout;

/** \brief Structure for parameters TO Host for a H263 decode task */
typedef struct t1xhv_vdc_h263_param_out {

    t_ushort_value error_type;             /**<\brief Return bitstream error type    */
    t_ushort_value picture_loss;           /**<\brief Picturee loss flags            */
    t_ushort_value slice_loss_first_mb[8]; /**<\brief Slice lost first macroblock    */
    t_ushort_value slice_loss_mb_num[8];   /**<\brief Slice loss MB number           */
    t_ushort_value concealed_mb_num;       /**<\brief NB of Concealed MacroBlock     */
    t_ushort_value concealed_vp_num;       /**<\brief NB of Concealed video packets  */
    t_ushort_value decoded_vp_num;         /**<\brief NB of video packets decoded    */

    t_ushort_value  reserved_1;             /**<\brief Reserved 32                    */    
    t_ulong_value  reserved_2;             /**<\brief Reserved 32                    */    

} ts_t1xhv_vdc_h263_param_out, *tps_t1xhv_vdc_h263_param_out;

/** \brief Structure for parameters FROM Host for a JPEG decode  task */
typedef struct t1xhv_vdc_jpeg_param_in {

    t_ushort_value frame_width;            /**<\brief Nb of pixel per line*/
    t_ushort_value frame_height;           /**<\brief Nb of line*/
    t_ushort_value nb_components;          /**<\brief Nb of components in the scan */

    t_ushort_value h_sampling_factor_y;     /**< \brief horizontal sampling factor of Y   */
    t_ushort_value v_sampling_factor_y;     /**< \brief vertical sampling factor of Y   */

    t_ushort_value h_sampling_factor_cb;    /**< \brief horizontal sampling factor of Cb */
    t_ushort_value v_sampling_factor_cb;    /**< \brief vertical sampling factor of Cb   */

    t_ushort_value h_sampling_factor_cr;    /**< \brief horizontal sampling factor of Cr */
    t_ushort_value v_sampling_factor_cr;    /**< \brief vertical sampling factor of Cr   */

    t_ushort_value downsampling_factor;     /**< \brief 1,1/2,1/4,1/8   */

    t_ushort_value restart_interval;        /**< \brief restart interval segment length (Ri)   */

    t_ushort_value progressive_mode;        /**< \brief SOF2    */

    t_ushort_value nb_scan_components;      /**< \brief number of image component in frame (Nf)   */

    t_ushort_value component_selector_y;    /**< \brief ==1 if y present in current scan   */
    t_ushort_value component_selector_cb;   /**< \brief ==1 if cb present in current scan   */
    t_ushort_value component_selector_cr;   /**< \brief ==1 if cr present in current scan   */

    t_ushort_value start_spectral_selection;   /**< \brief start of spectral selection
                                                *  in progressive mode (Ss)
                                                */
    t_ushort_value end_spectral_selection;     /**< \brief end of spectral selection
                                                *  in progressive mode (Se)
                                                */
    t_ushort_value successive_approx_position; /**< \brief Al value (low)   */
    t_ushort_value ace_enable;                 /**< \brief ask for 420 data processing:unused */
    t_ushort_value ace_strength;               /**< \brief ask for 420 data processing:unused */
    t_ushort_value reserved_1;                 /**<\brief Reserved 16             */
    t_ulong_value  reserved_2;                 /**<\brief Reserved 32             */

    t_ushort_value quant_y[64];                /**< \brief y quantization table   */
    t_ushort_value quant_cb[64];               /**< \brief cb quantization table   */
    t_ushort_value quant_cr[64];               /**< \brief cr quantization table   */

    t_ushort_value huffman_y_code_dc[12];      /**<\brief DC Huffman code table for luma   */
    t_ushort_value huffman_y_size_dc[12];      /**<\brief DC Huffman size table for luma   */
    t_ushort_value huffman_y_code_ac[256];     /**<\brief AC Huffman size table for luma   */
    t_ushort_value huffman_y_size_ac[256];     /**<\brief AC Huffman code table for luma   */

    t_ushort_value huffman_cb_code_dc[12];     /**<\brief DC Huffman size table for chroma */
    t_ushort_value huffman_cb_size_dc[12];     /**<\brief DC Huffman code table for chroma */
    t_ushort_value huffman_cb_code_ac[256];    /**<\brief AC Huffman size table for chroma */
    t_ushort_value huffman_cb_size_ac[256];    /**<\brief AC Huffman code table for chroma */

    t_ushort_value huffman_cr_code_dc[12];     /**<\brief DC Huffman size table for chroma */
    t_ushort_value huffman_cr_size_dc[12];     /**<\brief DC Huffman code table for chroma */
    t_ushort_value huffman_cr_code_ac[256];    /**<\brief AC Huffman size table for chroma */
    t_ushort_value huffman_cr_size_ac[256];    /**<\brief AC Huffman code table for chroma */
    t_ushort_value window_width;               /**<\brief Crop window width */
    t_ushort_value window_height;              /**<\brief Crop window height */
    t_ushort_value window_horizontal_offset;   /**<\brief offset of window width when there's a crop*/
    t_ushort_value window_vertical_offset;     /**<\brief offset of window height when there's a crop*/
    t_ulong_value reserved_3;                  /**<\brief Reserved 32             */
    t_ulong_value reserved_4;                  /**<\brief Reserved 32             */

} ts_t1xhv_vdc_jpeg_param_in, *tps_t1xhv_vdc_jpeg_param_in;


/** \brief Structure for output parameters of JPEG encode task */ 
typedef struct t1xhv_vdc_jpeg_param_out {

    t_ushort_value error_type;  /**<\brief Error status                           */
    t_ushort_value reserved_1;  /**<\brief Reserved 16                            */
    t_ushort_value ace_offset0; /**<\brief Automatic Contrast Enhancement offet 0 */
    t_ushort_value ace_offset1; /**<\brief Automatic Contrast Enhancement offet 1 */
    t_ushort_value ace_offset2; /**<\brief Automatic Contrast Enhancement offet 2 */
    t_ushort_value ace_offset3; /**<\brief Automatic Contrast Enhancement offet 3 */
    t_ulong_value  reserved_2;  /**<\brief Reserved 32                            */

} ts_t1xhv_vdc_jpeg_param_out, *tps_t1xhv_vdc_jpeg_param_out;

typedef struct t1xhv_vdc_jpeg_param_inout {

    t_ulong_value  mcu_index;         /**<\brief MCU index                              */
    t_ulong_value  end_of_band_run;   /**< \brief end of band value in progressive mode */
    t_ushort_value dc_predictor_y;    /**<\brief Luma DC ppredictor                     */
    t_ushort_value dc_predictor_cb;   /**<\brief Cb chroma DC predictor                 */
    t_ushort_value dc_predictor_cr;   /**<\brief Cr Chroma DC predictot                 */
    t_ushort_value reserved_1;        /**<\brief Reserved 32                            */
    t_ulong_value  ace_count0;        /**<\brief Automatic Contrast Enhancement offet 0 */
    t_ulong_value  ace_count1;        /**<\brief Automatic Contrast Enhancement offet 1 */
    t_ulong_value  ace_count2;        /**<\brief Automatic Contrast Enhancement offet 2 */
    t_ulong_value  ace_count3;        /**<\brief Automatic Contrast Enhancement offet 3 */
    t_ulong_value  crop_mcu_index;              /**<\brief MCU index in crop                      */
    t_ulong_value  crop_mcu_index_in_row;       /**<\brief MCU index in crop in row               */
    t_ulong_value  reserved_2;                  /**<\brief Reserved 32                            */
    t_ulong_value  reserved_3;                  /**<\brief Reserved 32                            */

} ts_t1xhv_vdc_jpeg_param_inout, *tps_t1xhv_vdc_jpeg_param_inout;


/* Avoid conflicts with codecs that have not yet splitten their interface */
#ifndef _T1XHV_HOST_INTERFACE_COMMON_H_

/** @{ \name enable_annexes parameter bitfield definition  
 *     \author Jean-Marc Volle
 *     \note Spec V0.95 p348
 */

/** \brief Enable Annex D.1:
 * As an input,EAD allows to enable the annex D.1
 * (motion vectors over picture boundaries)for an H263 decode
 * subtask.It is not used if picture_coding_type=0.In the profiles
 * that are currently supported,it must be equal to EAJ,otherwise
 * error_type is set to 0xc4. As an output,EAD returns the annex D.1
 * enable  ag for the next frame found in the bitstream,if enable_scd=1
 * (if enable_scd=0,this  eld is unde  ned).It is equal to the EAJ
 * output. 0 =annex disabled 1 =annex enabled   
 */
#define ENABLE_ANNEXES_EAD 0x0001 

/** \brief Enable Annex F.2:
 * As an input,EAF allows to enable the annex F.2 (four motion 
 * vectors per macroblock)for an H263 decode subtask.It is not 
 * used if picture_coding_type=0.In the pro  les that are currently
 * supported,it must be equal to EAJ,otherwise error_type is set to 
 * 0xc5. As an output,EAF returns the annex F.2 enable  ag for the 
 * next frame found in the bitstream,if enable_scd=1 (if enable_scd=0,
 * this  field is undefined).It is equal to the EAJ output.
 */
#define ENABLE_ANNEXES_EAF 0x0002 

/** \brief Enable Annex I:  
 * As an input,EAI allows to enable the annex I (advanced intra coding)
 * for an H263 decode subtask. As an output,EAI returns the annex I 
 * enable flag for the next frame found in the bitstream,if enable_scd=1
 * (if enable_scd=0,this field is undefined).It is obtained from the 
 * OPPTYPE  eld of the H263 bitstream.
 */
#define ENABLE_ANNEXES_EAI 0x0004 

/** \brief Enable Annex J:
 * As an input,EAJ allows to enable the annex J (deblocking  lter)for
 * an H263 decode subtask. As an output,EAJ returns the annex J enable
 * flag for the next frame found in the bitstream,if enable_scd=1 
 * (if enable_scd=0,this  eld is undefined).It is obtained from the 
 * OPPTYPE  eld of the H263 bitstream.
 */
#define ENABLE_ANNEXES_EAJ 0x0008 

/** \brief Enable Annex K:
 * As an input,EAK allows to enable the annex K (slice structured 
 * coding,with- out submodes)for an H263 decode subtask. As an output,
 * EAK returns the annex K enable flag for the next frame found in the
 * bitstream,if enable_scd=1 (if enable_scd=0,this  eld is undefined).
 * It is obtained from the OPPTYPE  eld of the H263 bitstream.   
 */
#define ENABLE_ANNEXES_EAK 0x0010 

/** \brief Enable Annex T:
 * As an input,EAT allows to enable the annex T (modi  ed quantization)
 * for an H263 decode subtask. As an output,EAT returns the annex T enable
 * flag for the next frame found in the bitstream,if enable_scd=1 
 * (if enable_scd=0,this  eld is undefined).It is obtained from 
 * the OPPTYPE  filed of the H263 bitstream.   */
#define ENABLE_ANNEXES_EAT 0x0020 
/** @}end of enable_annexes parameter bitfield definition*/



/*****************************************************************************/
/**
 * \brief  Parameter structure encode
 * \author Philippe Rochette
 *
 * Parameter structure for encode. Hamac Video Spec v0.1 sections 7.6
 **/
/*****************************************************************************/

/** \brief This structure define description of a subtask encode. */
typedef struct t1xhv_vec_subtask_param {

    ts_t1xhv_subtask_link s_link;                        /**<\brief  Link to next subtask (chained list) 
                                                          *              -- ts_t1xhv_subtask_link
                                                          */
    t_ahb_address         addr_in_frame_buffer;          /**<\brief  Add. of struct for input frame buffer
                                                          *              -- ts_t1xhv_vec_frame_buffer_in
                                                          */
    t_ahb_address         addr_out_frame_buffer;         /**<\brief  Add. of struct for output frame buffer 
                                                          *              -- ts_t1xhv_vec_frame_buffer_out
                                                          */
    t_ahb_address         addr_internal_buffer;          /**<\brief  Add. of struct for internal buffer 
                                                          *              -- ts_t1xhv_vec_internal_buf
                                                          */
    t_ahb_address         addr_in_header_buffer;         /**<\brief  Add. of struct for header buffer 
                                                          *              -- ts_t1xhv_bitstream_buf_header
                                                          */
    t_ahb_address         addr_in_bitstream_buffer;      /**<\brief  Add. of struct for in bitstr. buffer 
                                                          *              -- ts_t1xhv_init_bitstream_buffer
                                                          */
    t_ahb_address         addr_out_bitstream_buffer;     /**<\brief  Add. of struct for output bitstream buffer
                                                          *              -- ts_t1xhv_bitstream_buffer
                                                          */
    t_ahb_address         addr_in_parameters;            /**<\brief  Add. of struct for input parameters 
                                                          *              of encode (depend on standard)
                                                          */
    t_ahb_address         addr_out_parameters;           /**<\brief  Add. of struct for output parameters 
                                                          *              of encode (depend on standard)
                                                          */
    t_ahb_address         addr_in_frame_parameters;      /**<\brief  Add. of struct for inout parameters
                                                          *              of encode (depend on standard)
                                                          */
    t_ahb_address         addr_out_frame_parameters;     /**<\brief  Add. of struct for inout parameters
                                                          *              of encode (depend on standard)
                                                          */
    t_ahb_address         reserved_1;                    /**<\brief Reserved 32                            */
    t_ahb_address         reserved_2;                    /**<\brief Reserved 32                            */

} ts_t1xhv_vec_subtask_param, *tps_t1xhv_vec_subtask_param;


/** \brief This structure define a reference frame buffer. */
typedef struct t1xhv_vec_frame_buf_in {

    t_ahb_address  addr_source_buffer;        /**<\brief Buffer to encode.                    */
    t_ahb_address  addr_fwd_ref_buffer;       /**<\brief Address of prev reconstructed buffer */
    t_ahb_address  addr_grab_ref_buffer;      /**<\brief Address of buffer from grab          */
    t_ahb_address  addr_intra_refresh_buffer; /**<\brief Add. of intra refresh buffer         */

} ts_t1xhv_vec_frame_buf_in, *tps_t1xhv_vec_frame_buf_in;


/** \brief This structure define an output frame buffer. */
typedef struct t1xhv_vec_frame_buf_out {

    t_ahb_address  addr_dest_buffer;             /**<\brief Add. of output frame buffer  */
    t_ahb_address  addr_deblocking_param_buffer; /**<\brief Add. of parameters for PPP   */
    t_ahb_address  addr_motion_vector_buffer;    /**<\brief Add. of motion vector        */
    t_ahb_address  addr_intra_refresh_buffer;    /**<\brief Add. of intra refresh buffer */
    t_ahb_address  addr_ime_mv_field_buffer;     /**<\brief Add. of MV field buffer of IME */
    t_ahb_address  addr_ime_mv_field_buffer_end;     /**<\brief Add. of MV field buffer of IME */    
    t_ahb_address  reserved_2;     /**<\brief Add. of MV field buffer of IME */    
    t_ahb_address  reserved_3;     /**<\brief Add. of MV field buffer of IME */   
} ts_t1xhv_vec_frame_buf_out, *tps_t1xhv_vec_frame_buf_out;


/** \brief This structure define an internal frame buffer. */
typedef struct t1xhv_vec_internal_buf {

    t_ahb_address  addr_search_window_buffer;  /**<\brief Start add. of buffer for Search Window */
    t_ahb_address  addr_search_window_end;     /**<\brief End add. of buffer for Search Window   */
    t_ahb_address  addr_jpeg_run_level_buffer; /**<\brief Start add. of JPEG run level buffer    */
    t_ahb_address  addr_h264e_H4D_buffer;      /**<\brief Address of temporary buffer used by H4D for H264. */
    t_ahb_address  addr_h264e_rec_local;       /**<\brief Address of temporary buffer used for reconstruction local buffer for H264. */
    t_ahb_address  addr_h264e_metrics;         /**<\brief Address of temporary buffer used for coding choice and metrics for H264. */
    t_ahb_address  reserved_1;                 /**<\brief Reserved 32  */
    t_ahb_address  reserved_2;                 /**<\brief Reserved 32  */   
} ts_t1xhv_vec_internal_buf, *tps_t1xhv_vec_internal_buf;

#endif /* _T1XHV_HOST_INTERFACE_COMMON_H_ */

/** \brief This structure define parameters of a subtask encode for MPEG4. */
typedef struct t1xhv_vec_mpeg4_param_in {

    t_ushort_value picture_coding_type;      /**<\brief Type I or P of actual frame                 */
    t_ushort_value flag_short_header;        /**<\brief Short header mode if =1                     */
    t_ushort_value frame_width;              /**<\brief Width  in pixels from current frame         */
    t_ushort_value frame_height;             /**<\brief Height in pixels from current frame         */
    t_ushort_value window_width;             /**<\brief Width  in pixels from current Window        */
    t_ushort_value window_height;            /**<\brief Height in pixels from current Window        */
    t_ushort_value window_horizontal_offset; /**<\brief Horizontal offset from current Window       */
    t_ushort_value window_vertical_offset;   /**<\brief Vertical offset from current Window         */
    t_ushort_value gob_header_freq;          /**<\brief.Enables the use of GOB headers                */
    t_ushort_value gob_frame_id;             /**<\brief Species the gob_frame_id field of GOB headers */
    t_ushort_value data_partitioned;         /**<\brief enables data partitioning, for an MPEG4encode 
                                              * subtask. It is used only when flag_short_header=0.
                                              * It must be equal to 0 if the frame size is greater than 
                                              * CIF,otherwise error_type is set to 0xc6.
                                              */
    t_ushort_value reversible_vlc;           /**<\brief Enables the use of reversible codes,for an 
                                              * MPEG4encode subtask. It is used only when 
                                              * flag_short_header=0 and data_partitioned=1.
                                              */
    t_ushort_value hec_freq;                 /**<\brief Enables the use of Header Extension Codes and 
                                              * associated information,for an MPEG4encode subtask. It 
                                              * is used only when flag_short_header=0 and
                                              * data_partitioned=1. When hec_freq=0, no HEC information 
                                              * is inserted.Otherwise,HEC information is inserted in 
                                              * video packet headers once every hec_freq video packets.
                                              */
    t_ushort_value modulo_time_base;         /**<\brief the modulo_time_base field to be written in HEC   */
    t_ushort_value vop_time_increment;       /**<\brief the vop_time_increment field to be written in HEC */
    t_ushort_value vp_size_type;             /**<\brief Control of the video packet size,for an 
                                              * MPEG4encode subtask. It is used only when
                                              * flag_short_header=0 and data_partitioned=1.
                                              * It enables the use of vp_bit_size and vp_mb_size 
                                              * parameters.A video packet s closed as soon as it 
                                              * reaches the corresponding limit or the vp_size_max 
                                              * limit.Note that the last macroblock of the video packet
                                              * will be replaced by a "not coded" macroblock if the 
                                              * vp__size_max limit is reached.
                                              */
    t_ushort_value vp_size_max;              /**<\brief Maximum video packet size,in bits,for an 
                                              * MPEG4encode subtask. It is used only when 
                                              * flag_short_header=0 and data_partitioned=1.
                                              * If the vp_size_max limit is reached,the last macroblock 
                                              * is replaced by a "not coded" macroblock and the video
                                              * packet is closed,in order to respect the limit.
                                              */
    t_ushort_value vp_bit_size;              /**<\brief Minimum video packet size in bits,for an MPEG4
                                              * encode subtask.
                                              * It is used only when flag_short_header=0 and 
                                              * data_partitioned=1 and vp_size_type=0/2/3.
                                              * A video packet is closed as soon as it reaches the 
                                              * corresponding limit or the vp_size_max limit.
                                              * Note that the last macroblock of the video packet will 
                                              * be replaced by a "not coded" macroblock
                                              * if the vp_size_max limit is reached.
                                              */
    t_ushort_value vp_mb_size;               /**<\brief Minimum video packet size n macroblocks,
                                              * for an MPEG4encode subtask.It s used only when 
                                              * flag_short_header=0 and data_partitioned=1 and
                                              * vp_size_type=1/2/3.A video packet s closed as soon as 
                                              * it reaches the corresponding limit or the vp_size_max 
                                              * limit. Note that the last macroblock of the video 
                                              * packet will be replaced by a "not coded" macroblock 
                                              * if the vp__size_max limit is reached.
                                              */
    t_ushort_value init_me;                  /**<\brief Allows to initialize the motion estimation 
                                              * data at the beginning of an MPEG4/H263 encode
                                              * subtask (e.g.after a scene change detection)
                                              */
    t_ushort_value me_type;                  /**<\brief defines the motion estimation algorithm     */
    t_ushort_value vop_fcode_forward;        /**<\brief Fcode used (to determine Search window size */
    t_ushort_value rounding_type;            /**<\brief defines the value of the rounding control 
                                              * parameter used for pixel value interpolation
                                              * in motion compensation for P-frames. It is not 
                                              * used if picture_coding_type=0.
                                              */
    t_ushort_value intra_refresh_type;       /**<\brief enables the Adaptive Intra Refresh (AIR) 
                                              * and/or Cyclic Intra Refresh (CIR) algorithms,
                                              * for an MPEG4/H263 encode subtask.
                                              */
    t_ushort_value air_mb_num;               /**<\brief the number of macroblocks per frame to be 
                                              * refreshed in the AIR algorithm                 
                                              */
    t_ushort_value cir_period_max;           /**<\brief the maximum macroblock refresh period in
                                              * the CIR algorithm
                                              */
    t_ushort_value quant;                    /**<\brief Initial value of the quantization parameter 
                                              * for an MPEG4 or an H263 encode subtask. It must be 
                                              * different from 0, otherwise error_type is set to 0xc0.
                                              */
    t_ushort_value brc_type;                 /**<\brief the bit rate control (BRC) algorithm       */
    t_ulong_value  brc_frame_target;         /**<\brief Target size in bits for current frame. 
                                              * It is not used if brc_method=0/3.
                                              */
    t_ulong_value  brc_target_min_pred;      /**<\brief the predicted minimum number of bits to 
                                              * avoid buffer underflow
                                              */
    t_ulong_value  brc_target_max_pred;      /**<\brief the predicted maximum number of bits to 
                                              * avoid buffer overflow
                                              */
    t_ulong_value  skip_count;               /**<\brief the number of frames that have been 
                                              * skipped consecutively
                                              */

    t_ulong_value  bit_rate;                      /**<\brief Bitstream bit rate CBR/VBR */
    t_ushort_value framerate;                     /**<\brief Bitstream frame rate CBR/VBR */
    t_short_value  ts_modulo;           /**<\brief  vop time increment, signed */
    t_ushort_value ts_seconds;          /**<\brief  modulo time base */
    t_ushort_value air_thr;                  /**<\brief threshold for AIR */

    t_ulong_value  delta_target;                  /**<\brief Distance to target rate, signed */
    t_ushort_value minQp;                         /**<\brief Picture minimum allowed quantization parameter */
    t_ushort_value maxQp;                         /**<\brief Picture maximum allowed quantization parameter */
    t_ushort_value vop_time_increment_resolution; /**<\brief VOP time increment resolution CBR/VBR/HEC*/
    t_ushort_value fixed_vop_time_increment;      /**<\brief Fixed VOP time increment */
    t_ulong_value  Smax;                          /**<\brief Texture max size */
    t_ushort_value min_base_quality;              /**<\brief used in VBR only */
    t_ushort_value min_framerate;                 /**<\brief used in VBR only */
    t_ulong_value  max_buff_level;                /**<\brief used in CBR only */
  
  t_ushort_value	first_I_skipped_flag;  /**<\brief from mainver24d */
  t_short_value	init_ts_modulo_old;      /**<\brief from mainver24d */
  
  t_ushort_value slice_loss_first_mb[8];   /**<\brief the positions of the first macroblock of 
					    * slices that have been concealed                     */
    t_ushort_value slice_loss_mb_num[8];     /**<\brief number of macroblocks of slices that have
                                              * been concealed                                      */

} ts_t1xhv_vec_mpeg4_param_in, *tps_t1xhv_vec_mpeg4_param_in;


/** \brief This structure define parameters at the same time input
 * and output of a subtask encode for MPEG4. */
typedef struct t1xhv_vec_mpeg4_param_inout {

    t_ulong_value  bitstream_size;   /**<\brief Size in bits of the bitstream that has been 
                                       * written by an encode subtask, including the header but 
                                       * not the stuffing bits.
                                       */
    t_ulong_value  stuffing_bits;    /**<\brief Number of stuffing bits added in the bitstream 
                                       *  during the encode subtask.
                                       * It is not used if brc_method=0/1/3.
                                       */
    t_ulong_value  pictCount;         /**<\brief Picture count */
    t_ushort_value I_Qp;              /**<\brief Initial quantization parameter for intra picture */
    t_ushort_value P_Qp;              /**<\brief Initial quantization parameter for inter picture */
    t_ulong_value  last_I_Size;       /**<\brief Last intra picture size */
    t_ulong_value  comp_SUM;          /**<\brief comp sum */
    t_ulong_value  comp_count;        /**<\brief comp count */
    t_ulong_value  BUFFER_mod;        /**<\brief Buffer mod */
    t_ulong_value  ts_seconds_old;    /**<\brief Old modulo time base */
    t_short_value  ts_modulo_old;     /**<\brief Previous vop time increment, signed */
    t_ushort_value gov_flag;          /**<\brief for CBR */
    t_ulong_value  avgSAD;            /**<\brief Average SAD in VBR               */
    t_ulong_value  seqSAD;            /**<\brief Sequential SAD in VBR               */
    t_ushort_value min_pict_quality;  /**<\brief Minimum picture quality in VBR     */
    t_ushort_value diff_min_quality;  /**<\brief Difference minimum quality in VBR, signed  */
    t_ulong_value  TotSkip;           /**<\brief Total skip in VBR              */
  
    t_ulong_value  Skip_Current;      /**<\brief Used in VBR and CBR */
  
    t_ushort_value Cprev;             /**<\brief Previous header size in CBR                */
    t_ushort_value BPPprev;           /**<\brief Previous bit per pixel parameter in CBR    */
    t_ulong_value  PictQpSum;         /**<\brief Picture quantization parameter sum in CBR  */
    t_ulong_value  S_overhead;        /**<\brief Texture size overhead in CBR           */
  
    t_long_value   ts_vector[6];         /**<\brief for TS moving average */
  
    t_long_value   buffer_fullness;            /**<\brief for CBR */
    t_long_value   buffer_fullness_fake_TS;/**<\brief for CBR */
  
    t_ulong_value BUFFER_depletion; /**<\brief added from mainver2.4d */
  t_ushort_value  buffer_saved; /**<\brief added from mainver2.4d */
  t_ushort_value  intra_Qp_flag;   /**<\brief added for MAINVER2.5c */
    
  t_ulong_value BUFFER_depletion_fake_TS; /**<\brief added for MAINVER2.5a */
  t_ushort_value old_P_Qp_vbr;   /**<\brief added for MAINVER2.5c */
  t_ushort_value mv_field_index;  /**<\brief Added for IME to indicate the MV_field index */
  t_ulong_value pictCount_prev; /**<\brief added for MAINVER2.5c */
  t_ulong_value PictQpSumIntra; /**<\brief added for segmented mode */
  
  t_ushort_value hec_count;     /**<\brief number of VP since last HEC */
  t_ushort_value reserved_1;    /**<\brief Reserved to make a long */
  t_long_value reserved_2;		/**<\brief Reserved to make multiple for 4*16 */
  t_long_value reserved_3;		/**<\brief Reserved to make multiple for 4*16 */
  t_long_value reserved_4;		/**<\brief Reserved to make multiple for 4*16 */

	/* GT: enanched MB layer control for better Qp distribution, 02 Apr 07 */
    t_ushort_value BPPmbPrevAct;/**<\brief added for Enhanced MB quant control */
    t_ushort_value BPPmbPrevEst;/**<\brief added for Enhanced MB quant control */
    t_ushort_value CodedMB;/**<\brief added for Enhanced MB quant control */
    t_ushort_value CodedMB_flag;/**<\brief added for Enhanced MB quant control */
    t_long_value Lprev;/**<\brief added for Enhanced MB quant control */
    t_long_value BPPmbErr;/**<\brief added for Enhanced MB quant control */

} ts_t1xhv_vec_mpeg4_param_inout, *tps_t1xhv_vec_mpeg4_param_inout;


/** \brief This structure define parameters output of a subtask encode for MPEG4. */
typedef struct t1xhv_vec_mpeg4_param_out {

    t_ushort_value  error_type;     /**<\brief Error type if an error occurs during the encode 
                                     * subtask.
                                     */
    t_ushort_value  vp_num;         /**<\brief Number of video packets that have been written 
                                     * by an MPEG4 encode subtask.It is used only when 
                                     * flag_short_header=0. Note that there is no video packet
                                     * header for the first video packet.
                                     */
    t_ushort_value  vp_pos[32];      /**<\brief positions of the first video packets (up to 32)
                                     * that have been written by an MPEG4encode subtask. It is
                                     * used only when flag_short_header=0. The positions are
                                     * given in bytes,relatively to the beginning of the 
                                     * bitstream that has been written,including the header.
                                     */
    t_ushort_value  brc_skip_prev;   /**<\brief  Flag indicative when the encoded frame needs 
                                      * to be skipped */
    t_ushort_value  reserved_1;      /**<\brief reserved  16  */
    t_ulong_value   reserved_2;      /**<\brief reserved  32  */
    t_ulong_value   reserved_3;      /**<\brief reserved  32  */

 } ts_t1xhv_vec_mpeg4_param_out, *tps_t1xhv_vec_mpeg4_param_out;


/** \brief This structure define parameters of a subtask encode for H264. */
typedef struct t1xhv_vec_h264_param_in {

  /* the following are image parameters and can change from frame to frame */

  /* FRAME SIZE & CODING MODE */
  t_ushort_value  frame_width;                  /**<\brief Width  in pixels from current frame         */
  t_ushort_value  frame_height;                 /**<\brief Height in pixels from current frame         */
  t_ushort_value  window_width;                 /**<\brief Width  in pixels from current Window        */
  t_ushort_value  window_height;                /**<\brief Height in pixels from current Window        */
  t_ushort_value  window_horizontal_offset;     /**<\brief Horizontal offset from current Window       */
  t_ushort_value  window_vertical_offset;       /**<\brief Vertical offset from current Window         */

  t_ushort_value  picture_coding_type;          /**<\brief Type I or P of actual frame                 */
  t_ushort_value  idr_flag;                     /**<\brief Picture Intra type IDR                      */
  t_ulong_value   frame_num;                    /**<\brief Frame number                                */
  t_ushort_value  pic_order_cnt_type;           /**<\brief POC mode: 0,1,2                             */
  t_ushort_value  log2_max_frame_num_minus4;    /**<\brief log2 max frame num minus4                   */
  t_ushort_value  FirstPictureInSequence;       /**<\brief Flag to signal if it's the first frame in the sequence                 */
  
  /* SLICE CONTROL AND ERROR RESILIENCE */
  t_ushort_value  use_constrained_intra_flag;   /**<\brief 0: Inter MB pixels are allowed for intra prediction 1: Not allowed */
  t_ushort_value  slice_size_type;              /**<\brief control of the slice size                           */
  t_ushort_value  slice_byte_size;               /**<\brief  Argument to the specified slice algorithm           */
  t_ushort_value  slice_mb_size;                /**<\brief  Argument to the specified slice algorithm           */
  t_ushort_value  intra_refresh_type;           /**<\brief enables the Adaptive Intra Refresh (AIR) algorithm */
  t_ushort_value  air_mb_num;                   /**<\brief the number of macroblocks per frame to be refreshed by the AIR algorithm */
  t_ushort_value  mv_toogle;                /**<\brief  toogle to switch temporal MV field index in IME block */                   
  t_ushort_value  slice_loss_first_mb[8];       /**<\brief the positions of the first macroblock of slices that have been concealed */
  t_ushort_value  slice_loss_mb_num[8];         /**<\brief number of macroblocks of slices that have been concealed */


  t_ulong_value   MaxSumNumBitsInNALU ;         /** <\brief max size for a AU                          */

  
  /* the following parameters are to control the deblocking filter */
  t_ushort_value  disable_deblocking_filter_idc;        /**<\brief disable loop filter  */
  t_short_value   slice_alpha_c0_offset_div2;           /**<\brief custom loop filter parameter                */
  t_short_value   slice_beta_offset_div2;               /**<\brief custom loop filter parameter                */

  /* the following are specific for use with the rate-controller */
  t_ushort_value  brc_type;                 /**<\brief the bit rate control (BRC) algorithm       */
  t_ulong_value   lastBPAUts;               /**<\brief removal timestamp of last AU with BP SEI message associated with */
  t_ulong_value   NALfinal_arrival_time;    /**<\brief arrival time of previous frame. Used by CBR for dynamic bitrate change support. */
  t_ulong_value   NonVCLNALUSize;           /**<\brief size of non-VCL NALU (i.e. SPS, PPS, filler NALU,...) */
  
  t_ulong_value   CpbBufferSize;            /**<\brief size of CPB buffer. Used by VBR. */
  t_ulong_value   bit_rate;                 /**<\brief Target bitrate     */
  t_ulong_value   timestamp;                /**<\brief Timestamp value of current frame */
  t_ushort_value  framerate;                /**<\brief Target framerate     */
  
  t_ushort_value  TransformMode;            /**<\brief Controls the use of 4x4 and 8x8 transform mode */
  t_ushort_value  encoder_complexity;       /**<\brief encoder complexity control */
  t_ushort_value  searchWin;                /**<\brief CDME search window size */
  t_ushort_value  reserved2[6];

} ts_t1xhv_vec_h264_param_in, *tps_t1xhv_vec_h264_param_in;


/** \brief This structure define parameters at the same time input
 * and output of a subtask encode for H264. */
typedef struct t1xhv_vec_h264_param_inout {
  t_ushort_value   quant;                        /**<\brief Current quantization parameter */ 
  t_ushort_value  I_Qp;                         /**<\brief Quantization parameter of last encoded intra picture */
  t_ulong_value   bitstream_size;               /**<\brief Size in bits of the bitstream that has been written by an encode subtask, including the header but not the stuffing bits. */
  t_ulong_value   stuffing_bits;                /**<\brief Number of stuffing bits added in the bitstream during the encode subtask. */
  
  /* CDME */
  t_ushort_value  spatial_temporal_mv_flag;     /**<\brief Flag to signal which motion field buffer to use in CDME (toggled 0->1->0->.... for each subsequent frame */
                                            
  /* CBR & VBR */                           
  t_ushort_value  Skip_Current;                 /**<\brief current picture skip flag */
  t_ulong_value   last_I_Size;                  /**<\brief Last intra picture size */
  t_ulong_value   prev_pict_Qp;                 /**<\brief Picture Qp value of previous frame */
  t_long_value    bits_dec_buffer;              /**<\brief Fullness of decoder buffer (bits<<8 => 8 bits precision). Used in CBR and VBR to modelize decoder buffer */
                                            
  /* CBR only */                            
  t_ulong_value   comp_SUM;                     /**<\brief comp sum : used in CBR */
  t_ulong_value   comp_count;                   /**<\brief comp count : used in CBR */
  t_ushort_value  Skip_Next;                    /**<\brief next picture skip flag : used in CBR */
  t_ushort_value  Cprev;                        /**<\brief Previous header size in CBR                */
  t_ulong_value   PictQpSum;                    /**<\brief Picture quantization parameter sum in CBR  */
  t_ulong_value   S_overhead;                   /**<\brief Texture size overhead in CBR           */
  t_long_value    bits_enc_buffer;              /**<\brief Fullness of encoder buffer (bits). Used in CBR */


  /* VBR only */
  t_ulong_value   PictSizeFIFO[16];  /**<\brief FIFO storing the size of last VBR_ARRAY_SIZE-n frames*/
  t_ulong_value   reserved_VBR;
  t_ushort_value  reserved2_VBR;
  t_ushort_value  reserved3_VBR;
  t_ulong_value   reserved4_VBR[12];
  t_ulong_value   reserved5_VBR[3];

  t_ushort_value  ForceIntraRecoding;           /**<\brief Force the recoding of the same picture as INTRA (set by VBR) */

  /* AIR data */
  t_ushort_value  refreshed_mbs;                /**<\brief Number of refreshed mbs in current frame>*/

  /* TIMESTAMPS */
  t_long_value    timestamp_old;                /**<\brief Timestamp value of previous frame */
  t_ulong_value   removal_time;                 /**<\brief Removal time of current frame (nb. of ticks 1/framerate) (generated by BRC) */

  /* dynamic options */
  t_ulong_value	  old_bit_rate;                 /**<\brief bitrate value of previous picture (used for dynamic bitrate change) */
  t_ushort_value  old_framerate;                /**<\brief framerate value of previous picture (used for dynamic framerate change) */
  t_ushort_value  reserved3[3];
} ts_t1xhv_vec_h264_param_inout, *tps_t1xhv_vec_h264_param_inout;


/** \brief This structure define parameters output of a subtask encode for H264. */
typedef struct t1xhv_vec_h264_param_out {
  t_ushort_value  error_type;               /**<\brief Error type if an error occurs during the encode subtask. */
  t_ushort_value  reserved;                 /**<\brief Padding for 4*32 multiple struct size  */
  t_ulong_value   reserved_1[3];            /**<\brief Padding for 4*32 multiple struct size  */
} ts_t1xhv_vec_h264_param_out, *tps_t1xhv_vec_h264_param_out;

/** \brief This structure define parameters of a subtask encode for H263. */
typedef struct t1xhv_vec_h263_param_in {

    t_ushort_value picture_coding_type;      /**<\brief Type I or P of actual frame */
    t_ushort_value frame_width;              /**<\brief Width  in pixels from current frame */
    t_ushort_value frame_height;             /**<\brief Height in pixels from current frame */
    t_ushort_value window_width;             /**<\brief Width  in pixels from current Window */
    t_ushort_value window_height;            /**<\brief Height in pixels from current Window */
    t_ushort_value window_horizontal_offset; /**<\brief Horizontal offset from current Window */
    t_ushort_value window_vertical_offset;   /**<\brief Vertical offset from current Window */
    t_ushort_value enable_annexes;           /**<\brief - Enable mv over picture boundary 
                                              *         - Enable AC/DC prediction
                                              *         - Enable deblocking filter 
                                              *         - Enable slice structure
                                              *         - Enable modified quantization 
                                              */
    t_ushort_value gob_header_freq;          /**<\brief Frequency of GOB headers */
    t_ushort_value gob_frame_id;             /**<\brief GOB frame id (to be written into GOB hdrs) */
    t_ushort_value slice_size_type;          /**<\brief Parameter for annex k */
    t_ushort_value slice_bit_size;           /**<\brief */
    t_ushort_value slice_mb_size;            /**<\brief */
    t_ushort_value init_me;                  /**<\brief Allows to initialize the motion estimation 
                                              * data at the beginning of an MPEG4/H263 encode
                                              * subtask (e.g.after a scene change detection)
                                              */
    t_ushort_value me_type;                  /**<\brief Selects motion est algo */
    t_ushort_value reserved_1;
    t_ushort_value rounding_type;            /**<\brief Used for motion comp */
    t_ushort_value intra_refresh_type;       /**<\brief Intra refresh: AIR/CIR */
    t_ushort_value air_mb_num;               /**<\brief Nbr of AIR MBs */
    t_ushort_value cir_period_max;           /**<\brief CIR period */
    t_ushort_value quant;                    /**<\brief Initial value of the quantization parameter 
                                              * for an MPEG4 or an H263 encode subtask. It must be 
                                              * different from 0, otherwise error_type is set to 0xc0.
                                              */
    t_ushort_value brc_type;                 /**<\brief Method for bit rate control  */
    t_ulong_value  brc_frame_target;          /**<\brief Target size in bits for current frame. 
                                               * It is used if brc_type=1.
                                               */
    t_ulong_value  brc_target_min_pred;      /**<\brief internal variable */
    t_ulong_value  brc_target_max_pred;      /**<\brief internal variable */
    t_ulong_value  skip_count;               /**<\brief nb of consecutive skipped images */
    t_ulong_value  bitrate;                   /**<\brief target bitrate */
    t_ushort_value framerate;                 /**<\brief framerate  */
    t_ushort_value ts_modulo;                 /**<\brief current TS  */
    t_ushort_value ts_seconds;                /**<\brief current TS  */
    t_ushort_value air_thr;                   /**<\brief threshold for AIR  */
    t_ulong_value  delta_target;              /**<\brief internal */
    t_ushort_value minQp;                     /**<\brief min Qp  */
    t_ushort_value maxQp;                     /**<\brief max Qp  */
    t_ushort_value vop_time_increment_resolution; /**<\brief internal  */
    t_ushort_value fixed_vop_time_increment;      /**<\brief internal  */
    t_ulong_value  Smax;                       /**<\brief internal */
    t_ushort_value min_base_quality;           /**<\brief internal  */
    t_ushort_value min_framerate;             /**<\brief internal  */
    t_ulong_value  max_buff_level;              /**<\brief internal */
    t_ushort_value slice_loss_first_mb[8];   /**<\brief the positions of the first macroblock of 
                                              * slices that have been concealed                     */
    t_ushort_value slice_loss_mb_num[8];     /**<\brief number of macroblocks of slices that have
                                              *   been concealed                     */
} ts_t1xhv_vec_h263_param_in, *tps_t1xhv_vec_h263_param_in;

/** \brief This structure define parameters at the same time input
 * and output of a subtask encode for H263. */
typedef struct t1xhv_vec_h263_param_inout {
    t_ulong_value bitstream_size;            /**<\brief size of encoded stream */
    t_ulong_value stuffing_bits;             /**<\brief stuffing bits */
    t_ulong_value pictCount;                 /**<\brief internal */
    t_ushort_value I_Qp;                     /**<\brief internal */
    t_ushort_value P_Qp;                     /**<\brief internal */
    t_ulong_value last_I_size;               /**<\brief internal */
    t_ulong_value comp_SUM;                  /**<\brief internal */
    t_ulong_value comp_count;                /**<\brief internal */
    t_ushort_value BUFFER_mod;               /**<\brief internal */
    t_ushort_value ts_modulo_old;            /**<\brief internal */
    t_ulong_value ts_seconds_old;            /**<\brief internal */
    t_ulong_value avgSAD;                    /**<\brief internal */
    t_ulong_value seqSAD;                    /**<\brief internal */
    t_ushort_value min_pict_quality;         /**<\brief internal */
    t_ushort_value diff_min_quality;         /**<\brief internal */
    t_ulong_value TotSkip;                   /**<\brief internal */
    t_ulong_value SkipCurrent;               /**<\brief internal */
    t_ushort_value Cprev;                    /**<\brief internal */
    t_ushort_value BPPprev;                  /**<\brief internal */
    t_ulong_value PictQpSum;                 /**<\brief internal */
    t_ulong_value S_overhead;                /**<\brief Texture size overhead in CBR */  
    t_long_value  ts_vector[6];              /**<\brief for TS moving average */  
    t_long_value  buffer_fullness;           /**<\brief for CBR */
} ts_t1xhv_vec_h263_param_inout, *tps_t1xhv_vec_h263_param_inout;


/** \brief This structure define parameters output of H263 encode subtask */
typedef struct t1xhv_vec_h263_param_out {

    t_ushort_value error_type;      /**<\brief Error status */
    t_ushort_value slice_num;       /**<\brief */
    t_ushort_value slice_pos[32];   /**<\brief positions of the 1st slices (up to 32) */
    t_ushort_value  brc_skip_prev;   /**<\brief skip decided by BRC  */
    t_ushort_value  reserved_1;      /**<\brief reserved  16  */
    t_ulong_value  reserved_2;      /**<\brief reserved  32  */
    t_ulong_value  reserved_3;      /**<\brief reserved  32  */
} ts_t1xhv_vec_h263_param_out, *tps_t1xhv_vec_h263_param_out;

/** \brief Structure for parameters FROM Host for a JPEG encode task */
typedef struct t1xhv_vec_jpeg_param_in {

    t_ushort_value frame_width;                /**<\brief Nb of pixel per line                 */
    t_ushort_value frame_height;               /**<\brief Nb of line                           */
    t_ushort_value window_width;               /**<\brief Nb of pixels per line to be encoded  */
    t_ushort_value window_height;              /**<\brief Nb of lines to be encoded            */
    t_ushort_value window_horizontal_offset;   /**<\brief Nb of pixels for horizontal offset   */
    t_ushort_value window_vertical_offset;     /**<\brief Nb of pixels for vertical offset     */
    t_ushort_value sampling_mode;              /**<\brief Added in v0.96                       */
    t_ushort_value restart_interval;           /**<\brief Nb of MCUs between 2 restart markers */
    t_ushort_value quant_luma[64];             /**<\brief Quantization table for luma          */
    t_ushort_value quant_chroma[64];           /**<\brief Quantization table for chroma        */
    t_ushort_value huffman_luma_code_dc[12];   /**<\brief DC Huffman code table                */
    t_ushort_value huffman_luma_size_dc[12];   /**<\brief DC Huffman size table                */
    t_ushort_value huffman_luma_code_ac[256];  /**<\brief AC Huffman code table                */
    t_ushort_value huffman_luma_size_ac[256];  /**<\brief AC Huffman size table                */
    t_ushort_value huffman_chroma_code_dc[12]; /**<\brief DC Huffman code table                */
    t_ushort_value huffman_chroma_size_dc[12]; /**<\brief DC Huffman size table                */
    t_ushort_value huffman_chroma_code_ac[256];/**<\brief AC Huffman code table                */
    t_ushort_value huffman_chroma_size_ac[256];/**<\brief AC Huffman size table                */
    t_ushort_value last_slice;                 /**<\brief Added in v0.96                       */
    t_ushort_value enable_optimized_quant;     /**<\brief Added in v0.96                       */
    t_ushort_value target_bpp;                 /**<\brief Added in v0.96                       */
    t_ushort_value enable_optimized_huffman;   /**<\brief Added in v0.96                       */
    t_ushort_value rotation;                   /**<\brief 0=no rotate,1=rotate 90,2=rotate -90 */
    t_ushort_value reserved_1;                 /**<\brief reserved 32              */
    t_ulong_value  reserved_2;                 /**<\brief reserved 32              */
   
} ts_t1xhv_vec_jpeg_param_in, *tps_t1xhv_vec_jpeg_param_in;

/** \brief Structure for output parameters of JPEG encode task */ 
typedef struct t1xhv_vec_jpeg_param_out {

    t_ushort_value error_type;                 /**<\brief Error status                      */
    t_ushort_value reserved;                   /**<\brief To align next field  on 32b boundary */
    t_ulong_value  bitstream_size;             /**<\brief Size of encoded bitstream in bits */
    t_ulong_value  reserved_1;                 /**<\brief To align struct size on 128b           */
    t_ulong_value  reserved_2;                 /**<\brief To align struct size on 128b           */

} ts_t1xhv_vec_jpeg_param_out, *tps_t1xhv_vec_jpeg_param_out;

/** \brief Structure for output parameters of JPEG encode task */
typedef struct t1xhv_vec_jpeg_param_inout {

    t_ushort_value restart_mcu_count;          /**<\brief Added in v0.96 */
    t_ushort_value dc_predictor_y;             /**<\brief Added in v0.96 */
    t_ushort_value dc_predictor_cb;            /**<\brief Added in v0.96 */
    t_ushort_value dc_predictor_cr;            /**<\brief Added in v0.96 */
    t_ushort_value restart_marker_id;
    t_ushort_value reserved_1;                 /**<\brief To align struct on 128b */
    t_ulong_value  reserved_2;                 /**<\brief To align struct on 128b */

} ts_t1xhv_vec_jpeg_param_inout, *tps_t1xhv_vec_jpeg_param_inout;




/*****************************************************************************/
/**
 * \brief  Parameter structure for display
 * \author Jean-Marc Volle
 *
 * Parameter structure for display. Hamac Video Spec v0.1 
 */
/*****************************************************************************/
typedef struct t1xhv_dpl_subtask_param {
     
    ts_t1xhv_subtask_link s_link;            /**<\brief  Link to next subtask 
                                              * (chained list) same for all tasks
                                              * -- ts_t1xhv_subtask_link */
    t_ahb_address addr_in_frame_buffer;      /**<\brief  Address of structure for 
                                              * input frame buffer
                                              * -- ts_t1xhv_dpl_frame_buffer_in */
    t_ahb_address addr_out_frame_buffer;     /**<\brief  Address of structure for 
                                              * output frame buffer    
                                              * -- ts_t1xhv_dpl_frame_buffer_out */
    t_ahb_address addr_internal_buffer;      /**<\brief  Address of structure for 
                                              * internal buffer 
                                              * --  ts_t1xhv_dpl_interna_buf */
    t_ahb_address addr_in_parameters;        /**<\brief  Address of structure for 
                                              * input parameters 
                                              * -- ts_t1xhv_dpl_parameters_in */
    t_ahb_address addr_out_parameters;       /**<\brief  Address of structure for 
                                              * output parameters
                                              * -- ts_t1xhv_dpl_parameters_out */
    t_ahb_address addr_in_frame_parameters;  /**<\brief  Add. of struct for inout parameters
                                              * of display                     */
    t_ahb_address addr_out_frame_parameters; /**<\brief  Add. of struct for inout parameters
                                              * of display                     */

    t_ulong_value reserved_1;                /**<\brief reserved  32  */

} ts_t1xhv_dpl_subtask_param, *tps_t1xhv_dpl_subtask_param;


/**
 * \brief t1xhv_dpl_frame_buf_in, pointed by the second field of the parameter 
 * structure
 */
typedef struct t1xhv_dpl_frame_buf_in {

    t_ahb_address  addr_source_buffer;           /**<\brief Source buffer start address    */
    t_ahb_address  addr_deblocking_param_buffer; /**<\brief Deblocing parameters
                                                  * buffer start address                   */
    t_ulong_value reserved_1;                   /**<\brief reserved 32                    */
    t_ulong_value reserved_2;                   /**<\brief reserved 32                    */

} ts_t1xhv_dpl_frame_buf_in, *tps_t1xhv_dpl_frame_buf_in;

/**
 * \brief t1xhv_dpl_frame_buf_out, pointed by the third field of the parameter 
 * structure, same as the one for decoder, necessary to duplicate ?
 */
typedef struct t1xhv_dpl_frame_buf_out {

    t_ahb_address  addr_dest_buffer; /**<\brief Destination buffer start address */
    t_ulong_value reserved_1;        /**<\brief reserved 32                      */
    t_ulong_value reserved_2;        /**<\brief reserved 32                      */
    t_ulong_value reserved_3;        /**<\brief reserved 32                      */

} ts_t1xhv_dpl_frame_buf_out, *tps_t1xhv_dpl_frame_buf_out;

/**
 * \brief t1xhv_dpl_internal_buf, pointed by the fourth field of the parameter 
 * structure, same as the one for decoder, necessary to duplicate ?
 */
typedef struct t1xhv_dpl_internal_buf {

    t_ahb_address  addr_temp_buffer; /**<\brief temporary buffer start address */
    t_ulong_value reserved_1;        /**<\brief reserved 32                      */
    t_ulong_value reserved_2;        /**<\brief reserved 32                      */
    t_ulong_value reserved_3;        /**<\brief reserved 32                      */

} ts_t1xhv_dpl_internal_buf, *tps_t1xhv_dpl_internal_buf;

/**
 * \brief t1xhv_dpl_parameters_in, pointed by the fifth field of the parameter 
 * structure. Read from Host
 */
typedef struct t1xhv_dpl_param_in {

    t_ushort_value source_frame_width;                   /**<\brief YCbCr input pict width in pixels  */
    t_ushort_value source_frame_height;                  /**<\brief YCbCr input pict height in pixels */
    t_ushort_value output_format;                       /**<\brief Output format */
    t_ushort_value reserved;
  
  t_ulong_value reserved_1;
  t_ulong_value reserved_2;

} ts_t1xhv_dpl_param_in, *tps_t1xhv_dpl_param_in;

/**
 * \brief t1xhv_dpl_parameters_out, pointed by the sixth field of the parameter 
 * structure.
 */
typedef struct t1xhv_dpl_param_out {

    t_ushort_value error_type;  /**<\brief Error type    */
    t_ushort_value reserved_1;  /**<\brief reserved  16  */
    t_ulong_value  reserved_2;  /**<\brief reserved  32  */
    t_ulong_value  reserved_3;  /**<\brief reserved  32  */
    t_ulong_value  reserved_4;  /**<\brief reserved  32  */

} ts_t1xhv_dpl_param_out, *tps_t1xhv_dpl_param_out;

typedef struct t1xhv_dpl_param_inout {
    t_ulong_value  reserved_1;    /**<\brief reserved  32        */
    t_ulong_value  reserved_2;    /**<\brief reserved  32        */
    t_ulong_value  reserved_3;    /**<\brief reserved  32        */
    t_ulong_value  reserved_4;    /**<\brief reserved  32        */
} ts_t1xhv_dpl_param_inout, *tps_t1xhv_dpl_param_inout;


#ifndef _T1XHV_HOST_INTERFACE_COMMON_H_

/*****************************************************************************/
/**
 * \brief  Parameter structure for image stabilization
 * \author Serge Backert
 *
 * Parameter structure for image stabilization
 */
/*****************************************************************************/

/** \brief Structure for parameters FROM Host for an image stab. encode task */
typedef struct t1xhv_vec_stab_param_in {

    t_ushort_value frame_width;                 /**<\brief Nb of pixel per line   */  
    t_ushort_value frame_height;                /**<\brief Nb of line             */ 
    t_ushort_value zone_of_interest_bitmap[84]; /**<\brief Zone to consider for
                                                            stab vect computation */
    t_ulong_value  reserved_1;                  /**<\brief To align struct on 32b */
   
} ts_t1xhv_vec_stab_param_in, *tps_t1xhv_vec_stab_param_in;

/** \brief Structure for parameters TO Host from an image stab. encode task */
typedef struct t1xhv_vec_stab_param_out {

    t_ushort_value error_type;       /**<\brief Error status                      */
    t_ushort_value reserved_1;       /**<\brief reserved 16                 */
    t_short_value  stab_vector_x;    /**<\brief Stabilization vector x coordinate */
    t_short_value  stab_vector_y;    /**<\brief Stabilization vector y coordinate */
    t_ulong_value  reserved_2;       /**<\brief To align struct on 16 bytes       */
    t_ulong_value  reserved_3;       /**<\brief To align struct on 16 bytes       */
  
} ts_t1xhv_vec_stab_param_out, *tps_t1xhv_vec_stab_param_out;




/*****************************************************************************/
/**
 * \brief  Parameter structure for grab
 * \author Serge Backert, Loic Habrial
 *
 * Parameter structure for grab. Hamac Video Spec v0.1
 */
/*****************************************************************************/
typedef struct t1xhv_grb_subtask_param {

    ts_t1xhv_subtask_link s_link;             /**<\brief Link to next subtask (chained 
                                               *          list) same for all tasks
                                               * -- ts_t1xhv_subtask_link                     */
    t_ahb_address addr_in_frame_buffer;       /**<\brief Add. of structure for input 
                                               *          frame buffer 
                                               * -- ts_t1xhv_grb_frame_buffer_in              */
    t_ahb_address addr_out_frame_buffer;      /**<\brief Add. of structure for output 
                                               *         frame buffer
                                               * -- ts_t1xhv_grb_frame_buffer_out             */
    t_ahb_address addr_internal_buffer;       /**<\brief Add. of structure for internal 
                                               *         buffer    
                                               * -- ts_t1xhv_grb_internal_buffer              */
    t_ahb_address addr_in_parameters;         /**<\brief Add. of structure for input 
                                               *          parameters   
                                               * -- ts_t1xhv_grb_parameters_in                */
    t_ahb_address addr_out_parameters;        /**<\brief Add. of structure for output 
                                               *          parameters  
                                               * -- ts_t1xhv_grb_parameters_out               */
    t_ahb_address addr_in_frame_parameters;   /**<\brief  Add. of struct for inout parameters
                                               *           of encode (depend on standard)     */
    t_ahb_address addr_out_frame_parameters;  /**<\brief  Add. of struct for inout parameters
                                               *           of encode (depend on standard)     */

    t_ahb_address reserved_1;                 /**<\brief reserved 32                          */
     
} ts_t1xhv_grb_subtask_param, *tps_t1xhv_grb_subtask_param;

/**
 * \brief t1xhv_grb_frame_buf_in, pointed by the second field of the parameter 
 * structure
 */
typedef struct t1xhv_grb_frame_buf_in {

    t_ulong_value reserved_1;   /**<\brief reserved 32 */
    t_ulong_value reserved_2;   /**<\brief reserved 32 */
    t_ulong_value reserved_3;   /**<\brief reserved 32 */
    t_ulong_value reserved_4;   /**<\brief reserved 32 */

} ts_t1xhv_grb_frame_buf_in, *tps_t1xhv_grb_frame_buf_in;

/**
 * \brief t1xhv_grb_frame_buf_out, pointed by the third field of the parameter 
 * structure.
 */
typedef struct t1xhv_grb_frame_buf_out {

    t_ahb_address addr_dest_lc_buffer;       /**<\brief Start address of destination buffer
                                              * for a grab macroblock  */
    t_ahb_address addr_dest_raw_data_buffer; /**<\brief Start address of destination buffer
                                              * for a grab raw data    */
    t_ahb_address addr_dest_raw_data_end;    /**<\brief End address of destination buffer
                                              * for a grab raw data    */
    t_ulong_value reserved;                  /**<\brief reserved 32 */
    
} ts_t1xhv_grb_frame_buf_out, *tps_t1xhv_grb_frame_buf_out;

/**
 * \brief t1xhv_grb_internal_buf, pointed by the fourth field of the parameter 
 * structure.
 */
typedef struct t1xhv_grb_internal_buf {

    t_ulong_value reserved_1;   /**<\brief reserved 32 */
    t_ulong_value reserved_2;   /**<\brief reserved 32 */
    t_ulong_value reserved_3;   /**<\brief reserved 32 */
    t_ulong_value reserved_4;   /**<\brief reserved 32 */

} ts_t1xhv_grb_internal_buf, *tps_t1xhv_grb_internal_buf;

/**
 * \brief t1xhv_grb_parameters_in, pointed by the fifth field of the parameter 
 * structure. Read from Host.
 */
typedef struct t1xhv_grb_param_in {

    t_ushort_value source_frame_width;              /**<\brief Width of the source frame              */
    t_ushort_value source_frame_height;             /**<\brief Height of the source frame             */
    t_ushort_value source_window_width;             /**<\brief Width of the source window             */
    t_ushort_value source_window_height;            /**<\brief Height of the source window            */
    t_ushort_value source_window_horizontal_offset; /**<\brief Horizontal offset of the source window */
    t_ushort_value source_window_vertical_offset;   /**<\brief Vertical offset of the source window   */
    t_ushort_value resized_window_width;            /**<\brief Width of the resized window            */
    t_ushort_value resized_window_height;           /**<\brief Height of the resized window           */
    t_ushort_value interface_configuration;         /**<\brief Camera interface usage                 */
    t_ushort_value grab_sync_line;                  /**<\brief Grab_sync trigger line index           */
    t_ushort_value chroma_sampling_format;          /**<\brief Processing of chroma components        */
    t_ushort_value ace_enable;                      /**<\brief ace enable                             */
    t_ushort_value ace_strength;                    /**<\brief ace correction strength from 1 to 8    */
    t_ushort_value ace_range;                       /**<\brief ace range: 0=full, 1=reduced(BT601)    */
    t_ushort_value output_range;                    /**<\brief ace output range of the grab subtask   */
    t_ushort_value interlace_enable;                /**<\brief ccir interlace mode enable             */
    t_ushort_value field_sync;                      /**<\brief ccir field synchronization selection   */
    t_ushort_value raw_data_bpp;                    /**<\brief ccir 10-bit mode enable                */
    t_ulong_value  reserved_1;                      /**<\brief reserved 32                            */
    t_ulong_value  reserved_2;                      /**<\brief reserved 32                            */
    t_ulong_value  reserved_3;                        /**<\brief reserved 32                            */

} ts_t1xhv_grb_param_in, *tps_t1xhv_grb_param_in;


/** \brief Structure for parameters FROM and TO Host for a grab task */
typedef struct t1xhv_grb_param_inout {

    t_ulong_value  reserved_1;    /**<\brief reserved 32         */
    t_ushort_value ace_offset0;   /**<\brief ace output offset 0 */
    t_ushort_value ace_offset1;   /**<\brief ace output offset 1 */
    t_ushort_value ace_offset2;   /**<\brief ace output offset 2 */
    t_ushort_value ace_offset3;   /**<\brief ace output offset 3 */
    t_ulong_value  reserved_2;    /**<\brief reserved 32         */

} ts_t1xhv_grb_param_inout, *tps_t1xhv_grb_param_inout;


/**
 * \brief t1xhv_grb_parameters_out, pointed by the sixth field of the parameter 
 * structure.
 */
typedef struct t1xhv_grb_param_out {

    t_ushort_value error_type;    /**<\brief Error status                         */
    t_ushort_value field_number;  /**<\brief the number of the ccir field grabbed */
    t_time_stamp   time_stamp;    /**<\brief Execution time stamp                 */
    t_ulong_value  reserved_1;    /**<\brief reserved 32                          */
    t_ulong_value  reserved_2;    /**<\brief reserved 32                          */

} ts_t1xhv_grb_param_out, *tps_t1xhv_grb_param_out;


/*****************************************************************************/
/** @{ \name Parameters structures for tvout
 *     \author Jean-Marc Volle
 *     \note Spec V0.1
 */
/*****************************************************************************/

/** \brief This structure define description of a subtask tvout */

typedef struct t1xhv_tvd_subtask_param {

    ts_t1xhv_subtask_link s_link;        /**<\brief  Link to next subtask 
                                          * (chained list) same for all tasks
                                          * -- ts_t1xhv_subtask_link */
    t_ahb_address addr_in_frame_buffer;  /**<\brief  Address of structure for 
                                          * input frame buffer
                                          * -- ts_t1xhv_tvd_frame_buf_in */
    t_ahb_address addr_init_parameters;  /**<\brief  Address of structure for 
                                          * init parameters
                                          * -- ts_t1xhv_tvd_param_init */
    t_ahb_address addr_in_parameters;    /**<\brief  Address of structure for 
                                          * in parameters
                                          * -- ts_t1xhv_tvd_param_in */
    t_ahb_address reserved;              /**<\brief  Reserved 32         */

} ts_t1xhv_tvd_subtask_param, *tps_t1xhv_tvd_subtask_param;


/**
 * \brief t1xhv_tvd_frame_buf_in, pointed by the second field of the parameter 
 * structure
 */
typedef struct t1xhv_tvd_frame_buf_in {

    t_ahb_address addr_source_buffer;      /**<\brief Source buffer start address */ 
    t_ahb_address reserved_1;              /**<\brief Reserved 32                 */
    t_ahb_address reserved_2;              /**<\brief Reserved 32                 */
    t_ahb_address reserved_3;              /**<\brief Reserved 32                 */
    
} ts_t1xhv_tvd_frame_buf_in, *tps_t1xhv_tvd_frame_buf_in;
/**
 * \brief t1xhv_tvd_param_init, pointed by the third field of the parameter 
 * structure, 
 */

typedef struct t1xhv_tvd_param_init {
    t_ushort_value clock_signal_selection;            /**<\brief Clock edge selection signal      */
    t_ushort_value clock_edge_selection;             /**<\brief  Clock edge selection             */
    t_ushort_value interlace_enable;                 /**<\brief  Interlacing enable flag          */
    t_ushort_value number_of_lines;                  /**<\brief  Number of lines                  */
    t_ushort_value field1_blanking_start_line;       /**<\brief  Field1 blanking start line       */
    t_ushort_value field1_blanking_end_line;         /**<\brief  Field1 blanking end line         */
    t_ushort_value field2_blanking_start_line;       /**<\brief  Field2 blanking start line       */
    t_ushort_value field2_blanking_end_line;         /**<\brief  Field2 blanking end line         */
    t_ushort_value field1_identification_start_line; /**<\brief  Field1 identification start line */
    t_ushort_value field2_identification_start_line; /**<\brief  Field2 identification start line */
    t_ushort_value line_blanking_witdh;              /**<\brief  Line blanking width              */
    t_ushort_value active_line_width;                /**<\brief  Active line width                */
    t_ulong_value  reserved_1;                       /**<\brief  Reserved 32                      */
    t_ulong_value  reserved_2;                       /**<\brief  Reserved 32                      */

} ts_t1xhv_tvd_param_init, *tps_t1xhv_tvd_param_init;

/**
 * \brief t1xhv_tvd_parameters_in, pointed by the fourth field of the parameter 
 * structure. Read from Host
 */
typedef struct t1xhv_tvd_param_in {
    t_ushort_value source_frame_width;                        /**<\brief Source frame width                        */
    t_ushort_value source_frame_height;                       /**<\brief Source frame height                       */
    t_ushort_value source_window_width;                       /**<\brief Source window width                       */
    t_ushort_value field1_source_window_height;               /**<\brief Field1 source window height               */
    t_ushort_value field2_source_window_height;               /**<\brief Field2 source window height               */
    t_ushort_value source_window_horizontal_offset;           /**<\brief Source window horizontal offset           */
    t_ushort_value field1_source_window_vertical_offset;      /**<\brief Field1 source window vertical offset      */
    t_ushort_value field2_source_window_vertical_offset;      /**<\brief Field2 source window vertical offset      */
    t_ushort_value destination_window_horizontal_offset;      /**<\brief Destination window horizontal offset      */
    t_ushort_value field1_destination_window_vertical_offset; /**<\brief Field1 destination window vertical offset */
    t_ushort_value field2_destination_window_vertical_offset; /**<\brief Field2 destination window vertical offset */
    t_ushort_value background_y;                              /**<\brief Background luminance value                */
    t_ushort_value background_cb;                             /**<\brief Background Cb chrominance value           */
    t_ushort_value background_cr;                             /**<\brief Background Cr chrominance value           */
    t_ulong_value  reserved_1;                                /**<\brief  Reserved 32                              */
    
} ts_t1xhv_tvd_param_in, *tps_t1xhv_tvd_param_in;

/** @}end of tvout subtask structures definition */


typedef struct vdc_vc1_param_in
{
    t_ulong_value  frame_size;                /**< \brief size of the frame in bytes   */
    t_ushort_value max_picture_width;         /**< \brief maximum width of the picture (Annex J HORIZ_SIZE)   */
    t_ushort_value max_picture_height;        /**< \brief maximum height of the picture (Annex J VERT_SIZE)   */
    t_ushort_value profile;                   /**< \brief profile: 0 == SIMPLE, 1 == MAIN   */
    t_ushort_value quantizer;                 /**< \brief quantizer specifier (Annex J QUANTIZER)   */
    t_ushort_value dquant;                    /**< \brief macro-bloc quantization (Annex J DQUANT)   */
    t_ushort_value max_b_frames;              /**< \brief maximum number of consecutive b-frames (Annex J MAXBFRAMES)   */
    t_ushort_value multires_coding_enabled;   /**< \brief multi resolution coding used (Annex J MULTIRES)   */
    t_ushort_value extended_mv_enabled;       /**< \brief extended motion vectors used (Annex J EXTENDED_MV)   */
    t_ushort_value overlap_transform_enabled; /**< \brief overlaping transform used (Annex J OVERLAP)   */
    t_ushort_value syncmarker_enabled;        /**< \brief synchronisation markers used (Annex J SYNCMARKER)   */
    t_ushort_value rangered_enabled;          /**< \brief range reduction used (Annex J RANGERED)   */
    t_ushort_value frame_interpolation_enabled;       /**< \brief frame interpolation in picture header (Annex J FINTERPFLAG)   */
    t_ushort_value variable_size_transform_enabled;   /**< \brief variable size inverse transform used (Annex J VSTRANSFORM)   */
    t_ushort_value loop_filter_enabled;               /**< \brief in-the-loop filtering used (Annex J LOOPFILTER)   */
    t_ushort_value fast_uvmc_enabled;                   /**< \brief fast chroma motion compensention (Annex J FASTUVMC)   */
    t_ushort_value is_smpte_conformant;                 /**< \brief flag stating that the stream is conformant to SMPTE (reset of MV history not done) default:TRUE   */
    t_ulong_value  padding2;
    t_ulong_value  padding3;
    t_ulong_value  padding4;    

} ts_t1xhv_vdc_vc1_param_in, *tps_t1xhv_vdc_vc1_param_in;


/** \brief Hamac video vc1 decode output parameters 
 *  \note These parameters are picture layer parameters needed for post-processing*/ 
typedef struct vdc_vc1_param_out 
{
    t_ushort_value error_type;          /**<\brief Error status */
    t_ushort_value frame_interpolation_hint_enabled;  /**< \brief picture layer frame interpolation hint set (INTERPFRM)   */
    t_ushort_value range_reduction_frame_enabled;     /**< \brief picture layer frame rangered flag  (RANGEREDFRM)   */
    t_ushort_value b_fraction_numerator;              /**< \brief picture layer b fraction numerator (BFRACTION)  */
    t_ushort_value b_fraction_denominator;            /**< \brief picture layer b fraction denominatror (BFRACTION)  */
    t_ushort_value buffer_fullness;                   /**< \brief picture layer buffer fullness (BF)   */
    t_ushort_value picture_res;                       /**< \brief picture resolution: 1x1 == 0 2x1 == 1,1x2 == 2, 2x2 = 3  */
    t_ushort_value max_picture_width;                     /**< \brief true width of the decoded picture (including res)   */
    t_ushort_value max_picture_height;                    /**< \brief true height of the decoded picture   */
    t_ushort_value picture_width;                     /**< \brief true width of the decoded picture (including res)   */
    t_ushort_value picture_height;                    /**< \brief true height of the decoded picture   */
    t_ushort_value picture_type;                      /**< \brief picture type: I==0, P==1,B==2,BI==3,SKIPPED==4   */
    t_ulong_value  padding1;                        /**< \brief  Reserved 32                              */
    t_ulong_value  padding2;                        /**< \brief  Reserved 32                              */

} ts_t1xhv_vdc_vc1_param_out, *tps_t1xhv_vdc_vc1_param_out;

/** \brief Hamac video vc1 decode in / out parameters */ 
typedef struct vdc_vc1_param_inout
{
    t_ushort_value intensity_compensate_enabled;             /**< \brief true if last P frame decoded has intensity compensation set    */
    t_ushort_value last_ref_rangered_enabled;                /**< \brief true if last reference decoded has range reduction  set    */
    t_ushort_value previous_last_ref_rangered_enabled;       /**< \brief true if previous last reference decoded has range reduction  set    */
    t_ushort_value last_ref_interpolation_hint_enabled;      /**< \brief used to update output parameters of skipped images */
    t_ushort_value last_ref_buffer_fullness;/**< \brief used to pass buffer fullness of last decoded picture to skipped pictures   */
    t_ushort_value luma_scale;                               /**< \brief LUMSCALE value of last P frame decoded   */
    t_ushort_value luma_shift;                               /**< \brief LUMSHIFT value of last P frame decoded   */
    t_ushort_value rnd_ctrl;                                 /**< \brief RND control value (VC-1 8.3.7)   */
    t_ushort_value reference_resolution;                     /**< \brief reference picture resolution same type than output param*/          
    t_ushort_value padding1;
    t_ulong_value  padding2;
    t_ulong_value  padding3;
    t_ulong_value  padding4;

} ts_t1xhv_vdc_vc1_param_inout, *tps_t1xhv_vdc_vc1_param_inout;

#endif /* _T1XHV_HOST_INTERFACE_COMMON_H_ */

#endif /* __T1XHV_NMF_ENV */

#endif /* _T1XHV_HOST_INTERFACE_H_ */


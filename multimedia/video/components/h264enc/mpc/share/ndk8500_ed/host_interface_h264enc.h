/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _T1XHV_HOST_INTERFACE_H264ENC_H_
#define _T1XHV_HOST_INTERFACE_H264ENC_H_

#include "t1xhv_retarget.h"

/** \brief This structure define parameters of a subtask encode for H264. */
typedef struct t1xhv_vec_h264_param_in {

    t_long_value level_idc;                        /**<\brief level idc (required by CDME)                */

  /* the following are image parameters and can change from frame to frame */
    t_ushort_value  picture_coding_type;      /**<\brief Type I or P of actual frame                 */
    t_ushort_value  frame_width;              /**<\brief Width  in pixels from current frame         */
    t_ushort_value  frame_height;             /**<\brief Height in pixels from current frame         */
    t_ushort_value  window_width;             /**<\brief Width  in pixels from current Window        */
    t_ushort_value  window_height;            /**<\brief Height in pixels from current Window        */
    t_ushort_value  window_horizontal_offset; /**<\brief Horizontal offset from current Window       */
    t_ushort_value  window_vertical_offset;   /**<\brief Vertical offset from current Window         */
    t_ushort_value  algo_config;              /**<\brief 0b11 for performances , 
													unsetting bit 0 for complex intra in P slices , 
													unsetting bit 1 for complex inter  in P slices 	 */
    t_ulong_value   CodedPictureCounter;      /**<\brief Coded picture image counter                 */
    t_ulong_value   frame_poc;                /**<\brief Current picture POC                         */
    t_ulong_value   frame_num;                /**<\brief Frame number                                */
    t_ushort_value  init_me;                  /**<\brief Allows to initialize the motion estimation
                                              * data at the beginning of an MPEG4/H263/H264  encode
                                              * subtask (e.g.after a scene change detection) */

    /* the following are from the configuration file */
    t_ushort_value  me_type;                  /**<\brief M.E. algorithm selection                    */
    t_ushort_value  rounding_type;            /**<\brief MECC:Used for motion comp */
    t_ushort_value  annexb;                   /**<\brief AnnexB selection : 0:on, others:off         */
    t_ushort_value  use_constrained_intra_flag;  /**<\brief 0: Inter MB pixels are allowed for intra prediction 1: Not allowed */
    t_ushort_value  slice_size_type;          /* control of the slice size */
    t_ushort_value  slice_byte_size;           /* Argument to the specified slice algorithm */
    t_ushort_value  slice_mb_size;            /* Argument to the specified slice algorithm */

    t_ushort_value  intra_disable;            /**<\brief each bit disable a specific INTRA mode      */
    t_ushort_value  intra_refresh_type;       /**<\brief enables the Adaptive Intra Refresh (AIR)
                                              * and/or Cyclic Intra Refresh (CIR) algorithms,
                                              * for an MPEG4/H263/H264 encode subtask.
                                              */
    t_ushort_value  air_mb_num;               /**<\brief the number of macroblocks per frame to be
                                              * refreshed in the AIR algorithm
                                              */
  t_ushort_value  reserved_2;           
  t_ushort_value  slice_loss_first_mb[8];   /**<\brief the positions of the first macroblock of
                                              * slices that have been concealed                    */
  t_ushort_value  slice_loss_mb_num[8];     /**<\brief number of macroblocks of slices that have
                                              * been concealed                                     */
  t_ulong_value   MaxSumNumBitsInNALU ;     /** <\brief max size for a AU                          */

  /* the following are used to encode the Slice Header */
    t_ushort_value  idr_flag;                 /**<\brief Picture Intra type IDR                      */
    t_ushort_value  pic_order_cnt_type;       /**<\brief POC mode: 0,1,2                             */ 
    t_ushort_value  log2_max_frame_num_minus4;/**<\brief log2 max frame num minus4                   */
    t_ushort_value  log2_max_pic_order_cnt_lsb_minus4;/**<\brief log2 max pic order cnt lsb minus4   */

    /* the following parameter is to disable deblocking filter (not implemented yet)*/
    t_ushort_value  disable_deblocking_filter_idc;              /**<\brief disable loop filter  */
  t_short_value   slice_alpha_c0_offset_div2;              /**<\brief custom loop filter parameter                */
  t_short_value   slice_beta_offset_div2;               /**<\brief custom loop filter parameter                */



    /* the following are specific for use with the rate-controller */
    t_ushort_value  brc_type;                 /**<\brief the bit rate control (BRC) algorithm       */
  t_ulong_value   lastBPAUts;               /**<\brief removal timestamp of last AU with BP SEI message associated with */
  t_ulong_value   NALfinal_arrival_time;    /**<\brief arrival time of previous frame. Used by CBR for
                                                 dynamic bitrate change support.
                                             */

    /* NZ: the following are specific for SEI message computattion in hamac side */
  t_ulong_value   CpbBufferSize;            /**<\brief size of CPB buffer. Used by VBR. */
  t_ulong_value   bit_rate;                 /**<\brief Target bitrate     */
  t_short_value   SeinitialQP;              /**<\brief Initial quantization parameter for first intra picture */
  t_ushort_value  framerate;                /**<\brief Target framerate     */
  t_ulong_value   timestamp;                /**<\brief Timesatamp value of current frame */

  t_ulong_value   NonVCLNALUSize;           /**<\brief size of non-VCL NALU (i.e. SPS, PPS, filler NALU,...) */

  t_ulong_value   reserved_3[2];            /**<\brief Padding for 4*32 multiple struct size  */

} ts_t1xhv_vec_h264_param_in, *tps_t1xhv_vec_h264_param_in;


/** \brief This structure define parameters at the same time input
 * and output of a subtask encode for H264. */
typedef struct t1xhv_vec_h264_param_inout {
    /* RR Normally quant param is in param_in structure , to be checked */
  t_short_value  quant;            /**<\brief Current quantization parameter */
  t_ushort_value I_Qp;              /**<\brief Quantization parameter of last encoded intra picture */

    t_ulong_value  bitstream_size;   /**<\brief Size in bits of the bitstream that has been
                                       * written by an encode subtask, including the header but
                                       * not the stuffing bits.
                                       */
    t_ulong_value  stuffing_bits;    /**<\brief Number of stuffing bits added in the bitstream
                                       *  during the encode subtask.
                                       * It is not used if brc_method=0/1/3.
                                       */
  
  t_ulong_value  last_I_Size;       /**<\brief Last intra picture size */
  t_ulong_value  comp_SUM;          /**<\brief comp sum : used in VBR and CBR */
  t_ulong_value  comp_count;        /**<\brief comp count : used in VBR and CBR */
 
  t_ushort_value Skip_Current;      /**<\brief current picture skip flag : used in VBR and CBR */
  t_ushort_value Skip_Next;         /**<\brief next picture skip flag : used in CBR */
 
  t_ushort_value Cprev;             /**<\brief Previous header size in CBR                */
  t_ushort_value reserved_1;        /**<\brief Just to align on 32 bits boundary */
  t_ulong_value  PictQpSum;         /**<\brief Picture quantization parameter sum in CBR  */
  t_ulong_value  S_overhead;        /**<\brief Texture size overhead in CBR           */
  t_ulong_value  prev_pict_Qp;      /**<\brief Picture Qp value of previous frame */
  
  t_long_value   bits_enc_buffer;   /**<\brief Fullness of encoder buffer (bits). Used in CBR and VBR */
  t_long_value   bits_dec_buffer;   /**<\brief Fullness of decoder buffer (bits<<8 => 8 bits precision). 
                                      * Used in CBR and VBR to compute removal times 
                                      */

  /* VBR */
  t_ulong_value	 last_size;	/**<\brief Size of previous picture   */
  t_ushort_value last_was_I;	/**<\brief Previous INTRA picture flag   */
  t_ushort_value reserved_2;          /**<\brief to align on 32 bits */

    /* TIMESTAMPS */
  t_long_value   timestamp_old; /**<\brief Timesatamp value of previous frame */
  t_ulong_value  removal_time;  /**<\brief Removal time of current frame (nb. of ticks 1/framerate) (generated by BRC) */

  /* dynamic options */
  t_ushort_value  old_framerate;    /**<\brief framerate value of previous picture (used for dynamic framerate change) */
  t_ushort_value reserved_3;          /**<\brief to align on 32 bits */
  t_ulong_value	old_bit_rate;      /**<\brief bitrate value of previous picture (used for dynamic bitrate change) */

  t_ushort_value previous_MB_MV_num;  /**<\brief DF: check for Level 3.1 constraints (number of MV across MBs) */
    t_ushort_value CC_modulation;       /**< Parameter for the modulation of the thresholds in case of low/middle-low motion */ /* Used in CDME8815 */
  /* AIR data */
  t_ushort_value refreshed_mbs;       /**<\brief Number of refreshed mbs in current frame>*/

  t_ushort_value reserved_4[7];    /**<\brief Padding for 4*32 multiple struct size  */

} ts_t1xhv_vec_h264_param_inout, *tps_t1xhv_vec_h264_param_inout;


/** \brief This structure define parameters output of a subtask encode for H264. */
typedef struct t1xhv_vec_h264_param_out {
  t_ushort_value error_type;      /**<\brief Error type if an error occurs during the encode subtask. */
  t_ushort_value slice_num;       /**<\brief Number of slices that have been written by an H264 encode subtask. */
  //t_ulong_value  slice_pos[3200]; /**<\brief positions of the first slices (up to 3200 enough for HDTV) that have been written by an H264 encode subtask. */

  t_ulong_value  reserved_1[3];   /**<\brief Padding for 4*32 multiple struct size  */
} ts_t1xhv_vec_h264_param_out, *tps_t1xhv_vec_h264_param_out;

#endif /* _T1XHV_HOST_INTERFACE_H264ENC_H_ */


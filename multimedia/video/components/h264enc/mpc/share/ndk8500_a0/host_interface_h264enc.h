/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _T1XHV_HOST_INTERFACE_H264ENC_H_
#define _T1XHV_HOST_INTERFACE_H264ENC_H_

#include "t1xhv_retarget.h"
#include "fhe_common_type.h"
#include "fhe_common_type_vbr.h"

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
  t_ushort_value  mv_toggle;                /**<\brief  toggle to switch temporal MV field index in IME block */                   
  t_ushort_value  slice_loss_first_mb[8];       /**<\brief the positions of the first macroblock of slices that have been concealed */
  t_ushort_value  slice_loss_mb_num[8];         /**<\brief number of macroblocks of slices that have been concealed */


  t_ulong_value   MaxSumNumBitsInNALU ;         /** <\brief max size for a AU                          */

  
  /* the following parameters are to control the deblocking filter */
  t_ushort_value  disable_deblocking_filter_idc;        /**<\brief disable loop filter  */
  t_short_value   slice_alpha_c0_offset_div2;           /**<\brief custom loop filter parameter                */
  t_short_value   slice_beta_offset_div2;               /**<\brief custom loop filter parameter                */

  /* the following are specific for use with the rate-controller */
  t_ushort_value  brc_type;                 /**<\brief the bit rate control (BRC) algorithm       */
  t_ulong_value   NALfinal_arrival_time;    /**<\brief arrival time of previous frame. Used by CBR for dynamic bitrate change support. */
  t_ulong_value   NonVCLNALUSize;           /**<\brief size of non-VCL NALU (i.e. SPS, PPS, filler NALU,...) */
  
  t_ulong_value   CpbBufferSize;            /**<\brief size of CPB buffer. Used by VBR. */
  t_ulong_value   bit_rate;                 /**<\brief Target bitrate     */
  t_ulong_value   timestamp;                /**<\brief Timestamp value of current frame */
  t_ushort_value  framerate;                /**<\brief Target framerate     */
  
  t_ushort_value  TransformMode;            /**<\brief Controls the use of 4x4 and 8x8 transform mode */
  t_ushort_value  encoder_complexity;       /**<\brief encoder complexity control */
  t_ushort_value  searchWin;                /**<\brief CDME search window size */
  t_ushort_value  LowComplexity;            /**<\brief IME working, normal or less calculations */
  t_ushort_value  ForceIntrapred;           /**<\brief 0: No impact of initial code behaviour 1:I16x16, 2:I8x8, 3:I4x4, 4:all => ONLY for DEBUG */
  t_ushort_value  ProfileIDC;               /**<\brief Profile: 66=baseline, 100=high */
  t_ushort_value  VBRConfig;                /**<\brief Bit 0: MAD_ADAPT, Bit 1: LUMA_ADAP, Bit 2: AN
TI_SKIP_ADAP; O:unset, 1:set */
  ts_fps          FrameRate;
  t_ushort_value  QPISlice;
  t_ushort_value  QPPSlice;
//+ code for step 2 of CR 332873
  t_ushort_value  BRC_dynamic_change;
  t_ushort_value  CBR_simplified_algo;
  t_ushort_value  CBR_clipped;
  t_ushort_value  CBR_clipped_min_QP;
  t_ushort_value  CBR_clipped_max_QP;
  t_ushort_value  cir_mb_num;
  /* New parameters for MVC */
  t_ushort_value  MVC_encoding;            /**<\brief 0: no MVC activated; 7: MVC_temporal, 8: MVC_spatial */
  t_ushort_value  MVC_view;                /**<\brief 0: Main view; 1: second view */      
  t_ushort_value  MVC_anchor_picture_flag; /**<\brief  signal anchor picture for mv field reset and reordoring picture management */ 
  t_ushort_value  reserved3[5];
//- code for step 2 of CR 332873
} ts_t1xhv_vec_h264_param_in, *tps_t1xhv_vec_h264_param_in;


/** \brief This structure define parameters at the same time input
 * and output of a subtask encode for H264. */

#ifdef _MMDSP_CC 
#pragma align 2
#endif

typedef struct t1xhv_vec_h264_param_inout {
  t_ushort_value   quant;                        /**<\brief Current quantization parameter */ 
  t_ushort_value  I_Qp;                         /**<\brief Quantization parameter of last encoded intra picture */
  t_ulong_value   bitstream_size;               /**<\brief Size in bits of the bitstream that has been written by an encode subtask, including the header but not the stuffing bits. */
  t_ulong_value   stuffing_bits;                /**<\brief Number of stuffing bits added in the bitstream during the encode subtask. */
  
  /* CBR & VBR */                           
  t_ushort_value  reserved;                     /**<\brief Reserved */
  t_ushort_value  Skip_Current;                 /**<\brief current picture skip flag */
  t_ulong_value   last_I_Size;                  /**<\brief Last intra picture size */
  t_ushort_value  prev_pict_Qp;                 /**<\brief Picture Qp value of previous frame */
  t_ushort_value  reserved_3 ;                  /**<\brief Reserved 32 */ 
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

  t_ulong_value   PictSizeFIFO[4];              /**<\brief FIFO storing the size of last VBR_ARRAY_SIZE-n frames*/

  t_ulong_value   PrevFrameAverageMAD;          /**<\brief Average value of MAD of original frame */

  t_ushort_value  LumaAdaptOn;                  /**<\brief Use luma adaptation in next frame ? */

  t_ushort_value  RecodeOnThisGop;              /**<\brief Number of recode performed in this gop */
  t_ulong_value   AvgLuma_PrevPic[12];          /**<\brief 3x4 array (unrolled) to pass to next frame the avg luma of each sector */

  t_ushort_value  ForceIntraRecoding;           /**<\brief Force the recoding of the same picture as INTRA (set by VBR) */

  /* AIR data */
  t_ushort_value  refreshed_mbs;                /**<\brief Number of refreshed mbs in current frame>*/

  /* TIMESTAMPS */
  t_long_value    timestamp_old;                /**<\brief Timestamp value of previous frame */
  t_ulong_value   removal_time;                 /**<\brief Removal time of current frame (nb. of ticks 1/framerate) (generated by BRC) */

  /* dynamic options */
  t_ulong_value	  old_bit_rate;                 /**<\brief bitrate value of previous picture (used for dynamic bitrate change) */
  t_ushort_value  old_framerate;                /**<\brief framerate value of previous picture (used for dynamic framerate change) */
  t_ushort_value  reserved_4;                   /**<\brief reserved */
  /* new CBR */
  ts_rational     BufferFullness;
  t_ulong_value   lastBPAUts;               /**<\brief removal timestamp of last AU with BP SEI message associated with */
} ts_t1xhv_vec_h264_param_inout, *tps_t1xhv_vec_h264_param_inout;


/** \brief This structure define parameters output of a subtask encode for H264. */
typedef struct t1xhv_vec_h264_param_out {
  t_ushort_value  error_type;               /**<\brief Error type if an error occurs during the encode subtask. */
//+ code for step 2 of CR 332873
  t_ushort_value slice_num;                 /**<\brief Number of slices that have been written by an H264 encode subtask. */
  t_ulong_value  slice_pos[346];            /**<\brief positions of the first slices (up to 346 for 720p see standard A.3.3 calculation is (3600+245760)/(30*24)) that have been written by an H264 encode subtask. */
  t_ulong_value  reserved[1];               /**<\brief to be aligned 4*32 bits. */
//- code for step 2 of CR 332873
} ts_t1xhv_vec_h264_param_out, *tps_t1xhv_vec_h264_param_out;

#endif /* _T1XHV_HOST_INTERFACE_H264ENC_H_ */


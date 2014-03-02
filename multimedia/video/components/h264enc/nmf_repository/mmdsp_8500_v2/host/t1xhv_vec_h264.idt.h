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

/* Generated t1xhv_vec_h264.idt defined type */
#if !defined(_t1xhv_vec_h264_idt)
#define _t1xhv_vec_h264_idt

#include <host/t1xhv_vec.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_vec_h264_param_in {
  t_ushort_value frame_width;
  t_ushort_value frame_height;
  t_ushort_value window_width;
  t_ushort_value window_height;
  t_ushort_value window_horizontal_offset;
  t_ushort_value window_vertical_offset;
  t_ushort_value picture_coding_type;
  t_ushort_value idr_flag;
  t_ulong_value frame_num;
  t_ushort_value pic_order_cnt_type;
  t_ushort_value log2_max_frame_num_minus4;
  t_ushort_value FirstPictureInSequence;
  t_ushort_value use_constrained_intra_flag;
  t_ushort_value slice_size_type;
  t_ushort_value slice_byte_size;
  t_ushort_value slice_mb_size;
  t_ushort_value intra_refresh_type;
  t_ushort_value air_mb_num;
  t_ushort_value mv_toggle;
  t_ushort_value slice_loss_first_mb[8];
  t_ushort_value slice_loss_mb_num[8];
  t_ulong_value MaxSumNumBitsInNALU;
  t_ushort_value disable_deblocking_filter_idc;
  t_short_value slice_alpha_c0_offset_div2;
  t_short_value slice_beta_offset_div2;
  t_ushort_value brc_type;
  t_ulong_value NALfinal_arrival_time;
  t_ulong_value NonVCLNALUSize;
  t_ulong_value CpbBufferSize;
  t_ulong_value bit_rate;
  t_ulong_value timestamp;
  t_ushort_value framerate;
  t_ushort_value TransformMode;
  t_ushort_value encoder_complexity;
  t_ushort_value searchWin;
  t_ushort_value LowComplexity;
  t_ushort_value ForceIntrapred;
  t_ushort_value ProfileIDC;
  t_ushort_value VBRConfig;
  ts_fps FrameRate;
  t_ushort_value QPISlice;
  t_ushort_value QPPSlice;
  t_ushort_value BRC_dynamic_change;
  t_ushort_value CBR_simplified_algo;
  t_ushort_value CBR_clipped;
  t_ushort_value CBR_clipped_min_QP;
  t_ushort_value CBR_clipped_max_QP;
  t_ushort_value cir_mb_num;
  t_ushort_value MVC_encoding;
  t_ushort_value MVC_view;
  t_ushort_value MVC_anchor_picture_flag;
  t_ushort_value reserved3[5];
} ts_t1xhv_vec_h264_param_in;

typedef ts_t1xhv_vec_h264_param_in* tps_t1xhv_vec_h264_param_in;

typedef struct t_xyuv_ts_t1xhv_vec_h264_param_inout {
  t_ushort_value quant;
  t_ushort_value I_Qp;
  t_ulong_value bitstream_size;
  t_ulong_value stuffing_bits;
  t_ushort_value reserved;
  t_ushort_value Skip_Current;
  t_ulong_value last_I_Size;
  t_ushort_value prev_pict_Qp;
  t_ushort_value reserved_3;
  t_long_value bits_dec_buffer;
  t_ulong_value comp_SUM;
  t_ulong_value comp_count;
  t_ushort_value Skip_Next;
  t_ushort_value Cprev;
  t_ulong_value PictQpSum;
  t_ulong_value S_overhead;
  t_long_value bits_enc_buffer;
  t_ulong_value PictSizeFIFO[4];
  t_ulong_value PrevFrameAverageMAD;
  t_ushort_value LumaAdaptOn;
  t_ushort_value RecodeOnThisGop;
  t_ulong_value AvgLuma_PrevPic[12];
  t_ushort_value ForceIntraRecoding;
  t_ushort_value refreshed_mbs;
  t_long_value timestamp_old;
  t_ulong_value removal_time;
  t_ulong_value old_bit_rate;
  t_ushort_value old_framerate;
  t_ushort_value reserved_4;
  ts_rational BufferFullness;
  t_ulong_value lastBPAUts;
} ts_t1xhv_vec_h264_param_inout;

typedef ts_t1xhv_vec_h264_param_inout* tps_t1xhv_vec_h264_param_inout;

typedef struct t_xyuv_ts_t1xhv_vec_h264_param_out {
  t_ushort_value error_type;
  t_ushort_value slice_num;
  t_ulong_value slice_pos[346];
  t_ulong_value reserved[1];
} ts_t1xhv_vec_h264_param_out;

typedef ts_t1xhv_vec_h264_param_out* tps_t1xhv_vec_h264_param_out;

#endif

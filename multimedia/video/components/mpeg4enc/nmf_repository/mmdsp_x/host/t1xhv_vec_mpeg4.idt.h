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

/* Generated t1xhv_vec_mpeg4.idt defined type */
#if !defined(_t1xhv_vec_mpeg4_idt)
#define _t1xhv_vec_mpeg4_idt

#include <host/t1xhv_vec.idt.h>
#include <host/t1xhv_retarget.idt.h>

typedef struct t_xyuv_ts_t1xhv_vec_mpeg4_param_in {
  t_ushort_value picture_coding_type;
  t_ushort_value flag_short_header;
  t_ushort_value frame_width;
  t_ushort_value frame_height;
  t_ushort_value window_width;
  t_ushort_value window_height;
  t_ushort_value window_horizontal_offset;
  t_ushort_value window_vertical_offset;
  t_ushort_value gob_header_freq;
  t_ushort_value gob_frame_id;
  t_ushort_value data_partitioned;
  t_ushort_value reversible_vlc;
  t_ushort_value hec_freq;
  t_ushort_value modulo_time_base;
  t_ushort_value vop_time_increment;
  t_ushort_value vp_size_type;
  t_ushort_value vp_size_max;
  t_ushort_value vp_bit_size;
  t_ushort_value vp_mb_size;
  t_ushort_value init_me;
  t_ushort_value me_type;
  t_ushort_value vop_fcode_forward;
  t_ushort_value rounding_type;
  t_ushort_value intra_refresh_type;
  t_ushort_value air_mb_num;
  t_ushort_value cir_period_max;
  t_ushort_value quant;
  t_ushort_value brc_type;
  t_ulong_value brc_frame_target;
  t_ulong_value brc_target_min_pred;
  t_ulong_value brc_target_max_pred;
  t_ulong_value skip_count;
  t_ulong_value bit_rate;
  t_ushort_value framerate;
  t_short_value ts_modulo;
  t_ushort_value ts_seconds;
  t_ushort_value air_thr;
  t_ulong_value delta_target;
  t_ushort_value minQp;
  t_ushort_value maxQp;
  t_ushort_value vop_time_increment_resolution;
  t_ushort_value fixed_vop_time_increment;
  t_ulong_value Smax;
  t_ushort_value min_base_quality;
  t_ushort_value min_framerate;
  t_ulong_value max_buff_level;
  t_ushort_value first_I_skipped_flag;
  t_short_value init_ts_modulo_old;
  t_ushort_value slice_loss_first_mb[8];
  t_ushort_value slice_loss_mb_num[8];
  t_ulong_value yuvformat;
  t_ushort_value adv_intra_coding;
  t_ushort_value mod_quant_mode;
  t_ushort_value slice_mode;
  t_ushort_value deblock_filter;
  t_ushort_value h263_p3;
} ts_t1xhv_vec_mpeg4_param_in;

typedef ts_t1xhv_vec_mpeg4_param_in* tps_t1xhv_vec_mpeg4_param_in;

typedef struct t_xyuv_ts_t1xhv_vec_mpeg4_param_inout {
  t_ulong_value bitstream_size;
  t_ulong_value stuffing_bits;
  t_ulong_value pictCount;
  t_ushort_value I_Qp;
  t_ushort_value P_Qp;
  t_ulong_value last_I_Size;
  t_ulong_value comp_SUM;
  t_ulong_value comp_count;
  t_ulong_value BUFFER_mod;
  t_ulong_value ts_seconds_old;
  t_short_value ts_modulo_old;
  t_ushort_value gov_flag;
  t_ulong_value avgSAD;
  t_ulong_value seqSAD;
  t_ushort_value min_pict_quality;
  t_ushort_value diff_min_quality;
  t_ulong_value TotSkip;
  t_ulong_value Skip_Current;
  t_ushort_value Cprev;
  t_ushort_value BPPprev;
  t_ulong_value PictQpSum;
  t_ulong_value S_overhead;
  t_long_value ts_vector[6];
  t_long_value buffer_fullness;
  t_long_value buffer_fullness_fake_TS;
  t_ulong_value BUFFER_depletion;
  t_ushort_value buffer_saved;
  t_ushort_value intra_Qp_flag;
  t_ulong_value BUFFER_depletion_fake_TS;
  t_ushort_value old_P_Qp_vbr;
  t_ushort_value mv_field_index;
  t_ulong_value pictCount_prev;
  t_ulong_value PictQpSumIntra;
  t_ushort_value hec_count;
  t_ushort_value reserved_1;
  t_long_value reserved_2;
  t_long_value reserved_3;
  t_long_value reserved_4;
  t_ushort_value BPPmbPrevAct;
  t_ushort_value BPPmbPrevEst;
  t_ushort_value CodedMB;
  t_ushort_value CodedMB_flag;
  t_long_value Lprev;
  t_long_value BPPmbErr;
} ts_t1xhv_vec_mpeg4_param_inout;

typedef ts_t1xhv_vec_mpeg4_param_inout* tps_t1xhv_vec_mpeg4_param_inout;

typedef struct t_xyuv_ts_t1xhv_vec_mpeg4_param_out {
  t_ushort_value error_type;
  t_ushort_value vp_num;
  t_ushort_value vp_pos[32];
  t_ushort_value brc_skip_prev;
  t_ushort_value reserved_1;
  t_ulong_value reserved_2;
  t_ulong_value reserved_3;
} ts_t1xhv_vec_mpeg4_param_out;

typedef ts_t1xhv_vec_mpeg4_param_out* tps_t1xhv_vec_mpeg4_param_out;

typedef struct t_xyuv_motion_vector {
  t_sint16 x;
  t_sint16 y;
  t_sint8 hx;
  t_sint8 hy;
  t_uint8 valid;
  t_uint32 sad;
  t_uint8 zerovector;
} motion_vector;

#endif

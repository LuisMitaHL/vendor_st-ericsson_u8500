/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _T1XHV_HOST_INTERFACE_MPEG2DEC_H_
#define _T1XHV_HOST_INTERFACE_MPEG2DEC_H_

#include "t1xhv_retarget.h"

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

    t_ushort_value f_code_0_0;
    t_ushort_value f_code_0_1;
    t_ushort_value f_code_1_0;
    t_ushort_value f_code_1_1;
    
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
    t_ushort_value  error_map[225];    /**<\brief Return a 450B OMX error map (720p) */
    t_ushort_value  reserved_1;
    t_ulong_value   reserved_2;
    t_ulong_value   reserved_3;
    t_ulong_value   reserved_4;

} ts_t1xhv_vdc_mpeg2_param_out, *tps_t1xhv_vdc_mpeg2_param_out;

#endif /* _T1XHV_HOST_INTERFACE_MPEG2DEC_H_ */


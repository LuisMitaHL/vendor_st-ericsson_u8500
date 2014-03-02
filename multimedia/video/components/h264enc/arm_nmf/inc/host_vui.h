/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_VUI_H
#define HOST_VUI_H

#include "settings.h"
#include "types.h"
#include "host_types.h"


#if ENABLE_VUI == 1

/**
 * \brief Structure for HRD parameters within VUI 
 */
 
typedef struct  
{    
    t_uint16 cpb_cnt_minus1;
    t_uint16 bit_rate_scale;
    t_uint16 cpb_size_scale;
    t_uint32 bit_rate_value_minus1[MAX_CBP_CNT];
    t_uint32 cpb_size_value_minus1[MAX_CBP_CNT];
    t_uint16 cbr_flag[MAX_CBP_CNT];
    t_uint16 initial_cpb_removal_delay_length_minus1;
    t_uint16 cpb_removal_delay_length_minus1;
    t_uint16 dpb_output_delay_length_minus1;
    t_uint16 time_offset_length;
} t_hrd_param;



/**
 * \brief Structure for VUI parameters within SPS 
 */

typedef struct 
{
    t_uint16 aspect_ratio_info_present_flag;
    t_uint16 aspect_ratio_idc;
    t_uint32 sar_width;
    t_uint32 sar_height;
    
    t_uint16 overscan_info_present_flag;
    t_uint16 overscan_appropriate_flag;
    
    t_uint16 video_signal_type_present_flag;
    t_uint16 video_format;
    t_uint16 video_full_range_flag;
    t_uint16 colour_description_present_flag;
    t_uint16 colour_primaries;
    t_uint16 transfer_characteristics;
    t_uint16 matrix_coefficients;
    
    t_uint16 chroma_loc_info_present_flag;
    t_uint16 chroma_sample_loc_type_top_field;
    t_uint16 chroma_sample_loc_type_bottom_field;
    
    t_uint16 timing_info_present_flag;
    t_uint32 num_units_in_tick;    
    t_uint32 time_scale;           
    t_uint16 fixed_frame_rate_flag;

    /* hrd_parameters for nal */
    t_uint16 nal_hrd_parameters_present_flag;
    t_hrd_param nal_hrd_parameters;

    /* hrd_parameters for vcl */
    t_uint16 vcl_hrd_parameters_present_flag;        
    t_hrd_param vcl_hrd_parameters;

    t_uint16 low_delay_hrd_flag;
            
    t_uint16 pic_struct_present_flag;
    
    t_uint16 bitstream_restriction_flag;
    t_uint16 motion_vectors_over_pic_boundaries_flag;
    t_uint32 max_bytes_per_pic_denom;
    t_uint32 max_bits_per_mb_denom;
    t_uint32 log2_max_mv_length_horizontal;
    t_uint32 log2_max_mv_length_vertical;
    t_uint32 num_reorder_frames;
    t_uint32 max_dec_frame_buffering;
} t_vui_data;



t_uint16 GetVUI(t_bit_buffer *p_b, t_vui_data *p_vui); 

#endif

#endif


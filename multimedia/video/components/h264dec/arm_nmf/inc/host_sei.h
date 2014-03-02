/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_SEI_H
#define HOST_SEI_H

#include "settings.h"
#include "types.h"
#include "host_types.h"
#include "host_decoder.h"


#define BP_PAYLOAD_TYPE 0
#define PT_PAYLOAD_TYPE 1
#define RP_PAYLOAD_TYPE 6
#define PANSCAN_PAYLOAD_TYPE    2
#define STEREO_VIDEO_INFO_PAYLOAD_TYPE         21
#define FRAME_PACKING_ARRANGEMENT_PAYLOAD_TYPE 45

#define MAX_NUMCLOCK_TIMESTAMP 3

/**
 * \brief Stereo Video Info SEI message 
 */

typedef struct 
{
    t_uint16 field_views_flag;
    t_uint16 top_field_is_left_view_flag;
    t_uint16 current_frame_is_left_view_flag;
    t_uint16 next_frame_is_second_view_flag;
    t_uint16 left_view_self_contained_flag;
    t_uint16 right_view_self_contained_flag;
} t_stereo_video_info_SEI;

/**
 * \brief Frame Packing Arrangement SEI message 
 */

typedef struct 
{
    t_uint32 frame_packing_arrangement_id;
    t_uint16 frame_packing_arrangement_cancel_flag;
    t_uint16 frame_packing_arrangement_type;
    t_uint16 quincunx_sampling_flag;
    t_uint16 content_interpretation_type;
    t_uint16 spatial_flipping_flag;
    t_uint16 frame0_flipped_flag;
    t_uint16 field_views_flag;
    t_uint16 current_frame_is_frame0_flag;
    t_uint16 frame0_self_contained_flag;
    t_uint16 frame1_self_contained_flag;
    t_uint16 frame0_grid_position_x;
    t_uint16 frame0_grid_position_y;
    t_uint16 frame1_grid_position_x;
    t_uint16 frame1_grid_position_y;
    t_uint16 frame_packing_arrangement_reserved_byte;
    t_uint16 frame_packing_arrangement_repetition_period;
    t_uint16 frame_packing_arrangement_extension_flag;
} t_frame_packing_arrangement_SEI;


/**
 * \brief Pan Scan rect info SEI message 
 */

typedef struct 
{
    t_uint16 pan_scan_rect_id;
    t_uint16 pan_scan_rect_cancel_flag;
    t_sint16 pan_scan_rect_left_offset;
    t_sint16 pan_scan_rect_right_offset;
    t_sint16 pan_scan_rect_top_offset;
    t_sint16 pan_scan_rect_bottom_offset;
    t_uint16 pan_scan_rect_repetition_period;
} t_panscan_SEI;


/**
 * \brief Recovery Point SEI message 
 */

typedef struct 
{
    t_uint16 recovery_frame_cnt;
    t_uint16 exact_match_flag;
    t_uint16 broken_link_flag;
    t_uint16 changing_slice_group_idc;
        
} t_rp_SEI;


#if ENABLE_TIMING == 1

/**
 * \brief Buffering Period SEI message 
 */

typedef struct 
{
    t_uint16 seq_parameter_set_id;
    
    t_uint32 nal_initial_cpb_removal_delay[MAX_CBP_CNT];
    t_uint32 nal_initial_cpb_removal_delay_offset[MAX_CBP_CNT];
    
    t_uint32 vcl_initial_cpb_removal_delay[MAX_CBP_CNT];
    t_uint32 vcl_initial_cpb_removal_delay_offset[MAX_CBP_CNT];
} t_bp_SEI;



/**
 * \brief Picture Timing SEI message
 */

typedef struct 
{
    t_uint32 cpb_removal_delay;
    t_uint32 dpb_output_delay;
        
    t_uint16 pic_struct;
    t_uint16 clock_timestamp_flag[MAX_NUMCLOCK_TIMESTAMP];
    
    t_uint16 ct_type[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 nuit_field_based_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 counting_type[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 full_timestamp_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 discountinuity_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 cnt_dropped_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 n_frames[MAX_NUMCLOCK_TIMESTAMP];
                
    t_uint16 seconds_value[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 minutes_value[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 hours_value[MAX_NUMCLOCK_TIMESTAMP];
                
    t_uint16 seconds_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 minutes_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint16 hours_flag[MAX_NUMCLOCK_TIMESTAMP];
            
    t_sint16 time_offset[MAX_NUMCLOCK_TIMESTAMP];
} t_pt_SEI;



/**
 * \brief Structure containing timings information
 */
 
typedef struct 
{
    t_uint32 nAU;
    t_uint16 bp_flag;
    t_uint16 pt_flag;
    t_uint32 bits;
    t_uint32 bitrate;
    t_uint32 previous_cpb_removal_delay;
    t_uint32 cpb_removal_delay_correction;
   
    double tc; 
    
    double t_r;
    double t_rn;
    double t_rn_first;
    
    double t_ai;
    double t_ai_earliest;
    double t_af;
    
    double t_o_dpb;
    
} t_timings;

#endif


/**
 * \brief Structure containing parsed SEI messages and decoded information
 */
 
typedef struct
{
    t_rp_SEI rp_SEI;
    t_panscan_SEI  panscan_SEI;
    t_stereo_video_info_SEI         stereo_video_info_SEI;
    t_frame_packing_arrangement_SEI frame_packing_arrangement_SEI;

#if ENABLE_TIMING == 1    
    t_bp_SEI bp_SEI;
    t_pt_SEI pt_SEI;
    t_timings timings;
    
    t_seq_par *active_sp;
#endif

} t_SEI;



t_uint16 GetSEI(t_bit_buffer *p_b, t_dec *dec, t_SEI *p_sei);
t_uint16 GetPanScanRectInfo(t_bit_buffer *p_b, t_panscan_SEI *panscan_sei);

#if ENABLE_TIMING == 1
void InitTimings(t_SEI *p_SEI);
t_uint16 DecodeTime(t_SEI *p_SEI);
#endif 


#endif


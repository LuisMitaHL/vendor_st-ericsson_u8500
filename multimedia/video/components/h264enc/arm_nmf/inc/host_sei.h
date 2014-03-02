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


#define BP_PAYLOAD_TYPE 0
#define PT_PAYLOAD_TYPE 1
#define RP_PAYLOAD_TYPE 6

#define MAX_NUMCLOCK_TIMESTAMP 3



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

#if ENABLE_TIMING == 1    
    t_bp_SEI bp_SEI;
    t_pt_SEI pt_SEI;
    t_timings timings;
    
    t_seq_par *active_sp;
#endif

} t_SEI;



t_uint16 GetSEI(t_bit_buffer *p_b, t_seq_par *p_sp, t_SEI *p_sei);

#if ENABLE_TIMING == 1
void InitTimings(t_SEI *p_SEI);
t_uint16 DecodeTime(t_SEI *p_SEI);
#endif 


#endif


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_SEI_H
#define HOST_SEI_H

//#include "host_nal.h"
//#include "host_annexb.h"
//#include "host_nalucommon.h"

//#include "types.h"
//#include "host_decoder.h"
#include "h264enc_host_parset.h"
#include "h264enc_host_vlc.h"

#define NONVCL_BUFFER_SIZE 128

typedef enum {
  NON_VCL_NALU,
  VCL_NALU
} te_NALUflavour;

typedef struct 
{
  t_sint32 startcodeprefix_len;      /*! 4 for parameter sets and first slice in picture, 3 for everything else (suggested) */
  t_uint32 len;                 /*! Length of the NAL unit (Excluding the start code, which does not belong to the NALU) */
  t_uint32 max_size;            /*! Nal Unit Buffer size */
  t_sint32 nal_unit_type;            /*! NALU_TYPE_xxxx */
  t_sint32 nal_reference_idc;        /*! NALU_PRIORITY_xxxx */
  t_sint32 forbidden_bit;            /*! should be always FALSE */
  t_uint8 *buf;        /*! contains the first byte followed by the EBSP */
} ts_NALU, *tps_NALU;

extern t_uint8  rbsp[NONVCL_BUFFER_SIZE]; /* NZ: statically alloc'ed buffer for non-VCL NAL unit EXCEPT Filled Data NAL Units */

/* From defines.h */

/* NZ: When SEI are enabled defines which type of bitstream HRD compliancy (type 1 or 2) */
#define NAL_HRD_PARAMETERS_PRESENT_FLAG           1
#define VCL_HRD_PARAMETERS_PRESENT_FLAG           1
#define PIC_STRUCT_PRESENT_FLAG                   0
#define BITSTREAM_RESTRICTION_FLAG                1

/* NZ: SEI static buffer for payloads */
#define PT_SEI_RBSP_BUFFER_SIZE       128
#define BP_SEI_RBSP_BUFFER_SIZE       128
#define SV_SEI_RBSP_BUFFER_SIZE       128
#define FP_SEI_RBSP_BUFFER_SIZE       128

/* NZ: Accuracy control in VUI/SEI time base */
/* Number of Bits used to encode CPB_Removal_Delay in Picture Timing SEI Messages */
#define SEI_INITIAL_CPB_REMOVAL_DELAY_BITS        28
#define SEI_CPB_REMOVAL_DELAY_BITS                16
#define SEI_DPB_REMOVAL_DELAY_BITS                16
/* End of defines.h */

/*! definition of SEI payload type */
typedef enum {
    SEI_BUFFERING_PERIOD,
    SEI_PICTURE_TIMING,
    SEI_PAN_SCAN_RECT,
    SEI_FILLER_PAYLOAD,
    SEI_USER_DATA_REGISTERED_ITU_T_T35,
    SEI_USER_DATA_UNREGISTERED,
    SEI_RECOVERY_POINT,
    SEI_REC_PIC_MARKING_REPETITION,
    SEI_SPARE_PIC,
    SEI_SCENE_INFO,
    SEI_SUB_SEQ_INFO,
    SEI_SUB_SEQ_LAYER_CHARACTERISTICS,
    SEI_SUB_SEQ_CHARACTERISTICS,
    SEI_FULL_FRAME_FREEZE,
    SEI_FULL_FRAME_FREEZE_RELEASE,
    SEI_FULL_FRAME_SNAPSHOT,
    SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START,
    SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END,
    SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET,
    SEI_FILM_GRAIN_CHARACTERISTICS,
    SEI_DEBLOCKING_FILTER_DISPLAY_CHARACTERISTICS,
    SEI_STEREO_VIDEO_INFO,

    SEI_POST_FILTER_HINT,
    SEI_TONE_MAPPING_INFO,
    SEI_SCALABILITY_INFO,
    SEI_SUB_PIC_SCALABLE_LAYER,
    SEI_NON_REQUIRED_LAYER_REP,
    SEI_PRIORITY_LAYER_INFO,
    SEI_LAYERS_NOT_PRESENT,
    SEI_LAYER_DEPENDENCY_CHANGE,
    SEI_SCALABLE_NESTING,
    SEI_BASE_LAYER_TEMPORAL_HRD,
    SEI_QUALITY_LAYER_INTEGRETY_CHECK,
    SEI_REDUNDANT_PIC_PROPERTY,
    SEI_TL0_DEP_REP_INDEX,
    SEI_TL_SWITCHING_POINT,
    SEI_PARALLEL_DECODING_INFO,
    SEI_MVC_SCALABLE_NESTING,
    SEI_VIEW_SCALABILITY_INFO,
    SEI_MULTIVIEW_SCENE_INFO,
    SEI_MULTIVIEW_ACQUISITION_INFO,
    SEI_NON_REQUIERED_VIEW_COMPONENT,
    SEI_VIEW_DEPENDENCY_CHANGE,
    SEI_OPERATION_POINTS_NOT_PRESENT,
    SEI_BASE_VIEW_TEMPORAL_HRD,
    SEI_FRAME_PACKING_ARRANGEMENT,
    SEI_RESERVED_MESSAGE
} SEI_type;


/* 
* BRC (host side) <=========> SEI (host side) interface 
*/
typedef struct {
  t_sint16 InitBuffer;  /**< InitBuffer : != 0 to signal that current AU initialize the CPB */
  t_uint32 currAUts;    /**< currAUts   : timestamp in 1/frame_rate unit */ 
  t_uint32 NALAUsize;   /**< NALAUsize  : size (in bits) of all NALU in AU */
  t_uint32 VCLAUsize;   /**< VCLAUsize  : size (in bits) of VCL NALU only */
  t_uint32 lastBPAUts;
  t_uint16 forceBP;
} t_BRC_SEI;


#ifdef __cplusplus
extern "C" {
#endif

/*
*  function prototypes
*/
void InitSEIio(t_BRC_SEI* SEIio);
void InitSEIMessages(void);
void write_sei_message(t_uint8* destination, SEI_type payload_type);
void WriteSEI (t_BRC_SEI* SEIio);
void UpdateFinalArrivalTime(t_BRC_SEI* SEIio);
//t_uint16 WriteSeiNALU(void);
//t_uint16 estimate_sei_size (void);
t_uint16 estimate_sei_size (int);
void GenerateSEI_NALU (tps_NALU p_nalu);

#ifdef __cplusplus
}
#endif

/* defines */
#define MAX_NUMCLOCK_TIMESTAMP 3    /**<  taken from H.264 standard */

/* 
* structs
*/

/* Buffering Period struct */
typedef struct 
{
    t_uint16 seq_parameter_set_id;
    
    t_uint32 NALinitial_cpb_removal_delay[MAXIMUMVALUEOFcpb_cnt];
    t_uint32 NALinitial_cpb_removal_delay_offset[MAXIMUMVALUEOFcpb_cnt];

    t_uint32 VCLinitial_cpb_removal_delay[MAXIMUMVALUEOFcpb_cnt];
    t_uint32 VCLinitial_cpb_removal_delay_offset[MAXIMUMVALUEOFcpb_cnt];

    Bitstream data;
    t_uint16 payloadSize;

    double NAL_lastBPAUremoval_time;
    double VCL_lastBPAUremoval_time;

} t_buffering_period_sei;


/* Picture Timing struct */
typedef struct 
{
    t_uint32 cpb_removal_delay;
    t_uint32 dpb_output_delay;
    t_uint8 pic_struct;
    t_uint8  clock_timestamp_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  ct_type[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  nuit_field_based_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  counting_type[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  full_timestamp_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  discountinuity_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  cnt_dropped_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  n_frames[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  seconds_value[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  minutes_value[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  hours_value[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  seconds_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  minutes_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_uint8  hours_flag[MAX_NUMCLOCK_TIMESTAMP];
    t_sint32 time_offset[MAX_NUMCLOCK_TIMESTAMP];

    double VCLfinal_arrival_time[MAXIMUMVALUEOFcpb_cnt];
    double NALfinal_arrival_time[MAXIMUMVALUEOFcpb_cnt];

    Bitstream data;
    t_uint32 payloadSize;

} t_picture_timing_sei;

/* Stereo Video Info struct */
typedef struct 
{
    t_uint8 field_views_flag;
    t_uint8 top_field_is_left_view_flag;
    t_uint8 current_frame_is_left_view_flag;
    t_uint8 next_frame_is_second_view_flag;
    t_uint8 left_view_self_contained_flag;
    t_uint8 right_view_self_contained_flag;
} t_stereo_video_sei;

/* Frame Packing Arrangement struct */
typedef struct 
{
    t_uint16 frame_packing_arrangement_id;
    t_uint8  frame_packing_arrangement_cancel_flag;
    t_uint8  frame_packing_arrangement_type;
    t_uint8  quincunx_sampling_flag;
    t_uint8  content_interpretation_type;
    t_uint8  spatial_flipping_flag;
    t_uint8  frame0_flipped_flag;
    t_uint8  field_views_flag;
    t_uint8  current_frame_is_frame0_flag;
    t_uint8  frame0_self_contained_flag;
    t_uint8  frame1_self_contained_flag;
    t_uint8  frame0_grid_position_x;
    t_uint8  frame0_grid_position_y;
    t_uint8  frame1_grid_position_x;
    t_uint8  frame1_grid_position_y;
    t_uint8  frame_packing_arrangement_reserved_byte;
    t_uint16 frame_packing_arrangement_repetition_period;
    t_uint8  frame_packing_arrangement_extension_flag;
} t_frame_packing_sei;


extern t_uint8  picture_timing_rbsp[PT_SEI_RBSP_BUFFER_SIZE]; /* NZ: statically alloc'ed buffer for SEI NALU rbsp */
extern t_uint8  buffering_period_rbsp[BP_SEI_RBSP_BUFFER_SIZE]; /* NZ: statically alloc'ed buffer for SEI NALU rbsp */
extern t_uint8  stereo_video_rbsp[SV_SEI_RBSP_BUFFER_SIZE]; 
extern t_uint8  frame_packing_rbsp[FP_SEI_RBSP_BUFFER_SIZE];

#endif

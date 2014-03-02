/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _HOST_PARSET_H_
#define _HOST_PARSET_H_

//#include <assert.h>
#include "specific_frameinfo.idt"
#include "../../inc/host_decoder.h"
//#include "hcl_defs.h"
//#include "vte_globals.h"

#define MAXRBSPSIZE 64000

#ifdef __ndk8500_ed__
#define PROFILE_IDC	66
#else
#define PROFILE_IDC	100
#endif
//#define LEVEL_IDC		31
//#define LEVEL_IDC		40 /* Just to match ref encoder */

#define NALU_TYPE_SLICE    1
#define NALU_TYPE_DPA      2
#define NALU_TYPE_DPB      3
#define NALU_TYPE_DPC      4
#define NALU_TYPE_IDR      5
#define NALU_TYPE_SEI      6
#define NALU_TYPE_SPS      7
#define NALU_TYPE_PPS      8
#define NALU_TYPE_AUD      9
#define NALU_TYPE_EOSEQ    10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL     12

#define NALU_PRIORITY_HIGHEST     3
#define NALU_PRIORITY_HIGH        2
#define NALU_PRIRITY_LOW          1
#define NALU_PRIORITY_DISPOSABLE  0

#define MAXIMUMVALUEOFcpb_cnt   32

/* FREXT Profile IDC definitions */
#define FREXT_HP        100      /* YUV 4:2:0/8 "High" */
#define FREXT_Hi10P     110      /* YUV 4:2:0/10 "High 10" */
#define FREXT_Hi422     122      /* YUV 4:2:2/10 "High 4:2:2" */
#define FREXT_Hi444     144      /* YUV 4:4:4/12 "High 4:4:4" */

#define PIC_PARAMETER_SET_ID							0
#define DEBLOCKING_FILTER_CONTROL_PRESENT_FLAG	0
#define PIC_INIT_QP_MINUS26					0
#define DELTA_PIC_ORDER_ALWAYS_ZERO_FLAG			0
#define PIC_ORDER_PRESENT_FLAG						0
#define NUM_REF_IDX_L0_ACTIVE_MINUS1				0

#define ZEROBYTES_SHORTSTARTCODE 2 /* indicates the number of zero bytes in the short start-code prefix */

//#define MAX_REF_BITS	32		/* FP: this has to be checked !!! Temporary value. */
#define MAX_WIDTH	720		/* RR: SDTV max */
#define MAX_HEIGHT	480		/* RR: SDTV max */
//#define MAX_FRAME_SIZE_MBS	((MAX_WIDTH/MB_BLOCK_SIZE)*(MAX_HEIGHT/MB_BLOCK_SIZE))
#define MAX_STREAMBUF_SIZE	(MAX_WIDTH*MAX_HEIGHT*8)	/* FP: max bitstream size per slice, 4 == safety margin */

//#define USE_ASSERT
#ifdef USE_ASSERT
#define ASSERT(value) assert(value)
#else
#define ASSERT(value)
#endif /* _USE_ASSERT_ */

#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef unsigned char byte;    /* byte type definition */

typedef struct
{
  t_uint32  cpb_cnt_minus1;                        	      /* ue(v) */
  t_uint32  bit_rate_scale;               			         /* u(4) */
  t_uint32  cpb_size_scale;											/* u(4) */
  t_uint32  bit_rate_value_minus1 [MAXIMUMVALUEOFcpb_cnt];	/* ue(v) */
  t_uint32  cpb_size_value_minus1[MAXIMUMVALUEOFcpb_cnt];	/* ue(v) */
  t_uint32  cbr_flag[MAXIMUMVALUEOFcpb_cnt];            		/* u(1) */
  t_uint32  initial_cpb_removal_delay_length_minus1;        /* u(5) */
  t_uint32  cpb_removal_delay_length_minus1;                /* u(5) */
  t_uint32  dpb_output_delay_length_minus1;                 /* u(5) */
  t_uint32  time_offset_length;                             /* u(5) */
} hrd_parameters_t;


typedef struct
{
  t_sint32  			aspect_ratio_info_present_flag;        /* u(1) */
  t_uint32  			aspect_ratio_idc;                      /* u(8) */
  t_uint32  			sar_width;                             /* u(16) */
  t_uint32  			sar_height;                            /* u(16) */
  t_sint32  			overscan_info_present_flag;            /* u(1) */
  t_sint32  			overscan_appropriate_flag;             /* u(1) */
  t_sint32  			video_signal_type_present_flag;        /* u(1) */
  t_uint32  			video_format;                          /* u(3) */
  t_sint32  			video_full_range_flag;                 /* u(1) */
  t_sint32  			colour_description_present_flag;       /* u(1) */
  t_uint32  			colour_primaries;                      /* u(8) */
  t_uint32  			transfer_characteristics;					/* u(8) */
  t_uint32  			matrix_coefficients;							/* u(8) */
  t_sint32  			chroma_location_info_present_flag;		/* u(1) */
  t_uint32  			chroma_sample_loc_type_top_field;		/* ue(v) */
  t_uint32  			chroma_sample_loc_type_bottom_field;	/* ue(v) */
  t_sint32  			timing_info_present_flag;					/* u(1) */
  t_uint32  			num_units_in_tick;                     /* u(32) */
  t_uint32  			time_scale;                            /* u(32) */
  t_sint32  			fixed_frame_rate_flag;                 /* u(1) */
  t_sint32  			nal_hrd_parameters_present_flag;       /* u(1) */
  hrd_parameters_t	nal_hrd_parameters;                    /* hrd_paramters_t */
  t_sint32      		vcl_hrd_parameters_present_flag;       /* u(1) */
  hrd_parameters_t 	vcl_hrd_parameters;                    /* hrd_paramters_t */
  /* if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag)) */
    t_sint32      	low_delay_hrd_flag;                    /* u(1) */
  /* NZ */
  t_sint32      		pic_struct_present_flag;               /* u(1) */
  /* NZ */
  t_sint32      		bitstream_restriction_flag;            /* u(1) */
  t_sint32      		motion_vectors_over_pic_boundaries_flag;	/* u(1) */
  t_uint32  			max_bytes_per_pic_denom;                  /* ue(v) */
  t_uint32  			max_bits_per_mb_denom;                    /* ue(v) */
  t_uint32  			log2_max_mv_length_vertical;              /* ue(v) */
  t_uint32  			log2_max_mv_length_horizontal;            /* ue(v) */
  t_uint32  			num_reorder_frames;                       /* ue(v) */
  t_uint32  			max_dec_frame_buffering;                  /* ue(v) */
} vui_seq_parameters_t;


typedef struct
{
  t_sint32	Valid;												/* indicates the parameter set is valid */
  t_uint32	seq_parameter_set_id;							/* ue(v) */
  t_sint32	entropy_coding_mode_flag;						/* u(1) */

  t_sint32	pic_scaling_matrix_present_flag;				/* u(1) */
  t_sint32	pic_scaling_list_present_flag[8];			/* u(1) */

  t_sint32	pic_init_qs_minus26;								/* se(v) */
  t_sint32	chroma_qp_index_offset;							/* se(v) */

  t_sint32	deblocking_filter_control_present_flag;	/* u(1) */
  t_sint32	constrained_intra_pred_flag;					/* u(1) */
  t_sint32	redundant_pic_cnt_present_flag;				/* u(1) */
  t_sint32	vui_pic_parameters_flag;						/* u(1) */
} pic_parameter_set_rbsp_t;


#define MAXnum_ref_frames_in_pic_order_cnt_cycle  256
typedef struct
{
  t_sint32	Valid;                  														/* indicates the parameter set is valid */
  
  t_uint32	profile_idc;                                      						/* u(8) */
  t_sint32  constrained_set0_flag;                            						/* u(1) */
  t_sint32  constrained_set1_flag;                            						/* u(1) */
  t_sint32  constrained_set2_flag;                            						/* u(1) */
  t_sint32  constrained_set3_flag;                            						/* u(1) */
  t_uint32  level_idc;                                        						/* u(8) */
  t_uint32  seq_parameter_set_id;                             						/* ue(v) */

  t_sint32	seq_scaling_list_present_flag[8];                 						/* u(1) */

  t_uint32  bit_depth_luma_minus8;                            						/* ue(v) */
  t_uint32  bit_depth_chroma_minus8;                          						/* ue(v) */
  t_sint16	log2_max_frame_num_minus4;                        						/* ue(v) */
  t_uint32	pic_order_cnt_type;
  /* if( pic_order_cnt_type == 0 )  */
  t_sint16 	log2_max_pic_order_cnt_lsb_minus4;                 					/* ue(v) */
  /* else if( pic_order_cnt_type == 1 ) */
  t_sint32	offset_for_non_ref_pic;                         						/* se(v) */
  t_uint32	num_ref_frames_in_pic_order_cnt_cycle;          						/* ue(v) */
    /* for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ ) */
  t_sint32	offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   /* se(v) */
  t_sint32	gaps_in_frame_num_value_allowed_flag;             						/* u(1) */
  t_uint32  pic_width_in_mbs_minus1;                          						/* ue(v) */
  t_uint32  pic_height_in_map_units_minus1;                   						/* ue(v) */
  t_sint32	frame_cropping_flag;                             						/* u(1) */
  t_uint32  frame_cropping_rect_left_offset;                						/* ue(v) */
  t_uint32  frame_cropping_rect_right_offset;               						/* ue(v) */
  t_uint32  frame_cropping_rect_top_offset;                 						/* ue(v) */
  t_uint32  frame_cropping_rect_bottom_offset;              						/* ue(v) */
  t_sint32	vui_parameters_present_flag;                      						/* u(1) */
  vui_seq_parameters_t vui_seq_parameters;                  						/* vui_seq_parameters_t */
} seq_parameter_set_rbsp_t;

/* Bitstream */
typedef struct
{
  t_sint32           byte_pos;           /* current position in bitstream; */
  t_sint32           bits_to_go;         /* current bitcounter */
  t_uint16           byte_buf;           /* current buffer for last written byte */
  t_uint8            *streamBuffer;      /* actual buffer for written bytes */
} Bitstream;

typedef struct 
{
  t_sint32 startcodeprefix_len;      	/*! 4 for parameter sets and first slice in picture, 3 for everything else (suggested) */
  t_uint32 len;                 			/*! Length of the NAL unit (Excluding the start code, which does not belong to the NALU) */
  t_uint32 max_size;            			/*! Nal Unit Buffer size */
  t_sint32 nal_unit_type;            	/*! NALU_TYPE_xxxx */
  t_sint32 nal_reference_idc;        	/*! NALU_PRIORITY_xxxx */
  t_sint32 forbidden_bit;            	/*! should be always FALSE */
  byte *buf;        							/*! conjtains the first byte followed by the EBSP */
} NALU_t;


//extern pic_parameter_set_rbsp_t *active_pps;
//extern seq_parameter_set_rbsp_t *active_sps;

/*--------------------------------------------------------------------------*
 * Functions                                                               		       *
 *--------------------------------------------------------------------------*/

NALU_t *AllocNALU(t_uint32 buffersize);
void FreeNALU(NALU_t *n);
#ifdef __cplusplus
extern "C" {
#endif
t_uint16 getIndexFromLevel (t_uint16 level);
#ifdef __cplusplus
}
#endif

void no_mem_exit(char *where);

t_sint32 RBSPtoEBSP(byte *streamBuffer, t_sint32 begin_bytepos, t_sint32 end_bytepos, t_sint32 min_num_bytes);
void SODBtoRBSP(Bitstream *currStream);
t_sint32 RBSPtoNALU (char *rbsp, 
							NALU_t *nalu,
							t_sint32 rbsp_size,
							t_sint32 nal_unit_type,
							t_sint32 nal_reference_idc,
							t_sint32 min_num_bytes,
							t_sint32 UseAnnexbLongStartcode);

NALU_t *GenerateSeq_parameter_set_NALU (seq_parameter_set_rbsp_t *active_sps);
NALU_t *GeneratePic_parameter_set_NALU (pic_parameter_set_rbsp_t *active_pps);

/* The following are local helpers, but may come handy in the future, hence public */
#ifdef __cplusplus
extern "C" {
#endif
void FillParameterSetStructures (seq_parameter_set_rbsp_t *sps, pic_parameter_set_rbsp_t *pps, int level_idc, t_specific_frameinfo* info, ts_t1xhv_vec_h264_param_in* h264_encode_par_in_nb);
t_sint32 GenerateSeq_parameter_set_rbsp (seq_parameter_set_rbsp_t *sps, char *buf);
t_sint32 GeneratePic_parameter_set_rbsp (pic_parameter_set_rbsp_t *pps, char *buf);
NALU_t* GenerateSEI_NALU_(int buf_period, int pic_num);
#ifdef __cplusplus
}
#endif


#endif

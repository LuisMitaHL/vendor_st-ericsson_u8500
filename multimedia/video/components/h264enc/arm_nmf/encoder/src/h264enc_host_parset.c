/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//#include <stdlib.h>
//#include <assert.h>
//#include <string.h>
//#include <alloca.h>

#include "types.h"
#include "specific_frameinfo.idt"
//#include "vte_sva_alloc.h"
#include "host_hamac_interface.h"
#include "h264enc_host_parset.h"
#include "h264enc_host_vlc.h"

#include "local_alloc.h"

#define malloc_(a)  mallocCM(a, 8-1)

#define FALSE 0

void *my_vfm_memory_ctxt;

/* NZ */
/* NS: modified (added first parameter) */
static t_sint32 GenerateVUISequenceParameters(seq_parameter_set_rbsp_t *sps, Bitstream *bitstream);
t_uint32 getMaxCPB(t_uint16 level);
/* NZ */

const t_uint32 LevelLimits[16][6] = {
  { 10,   1485,   99,      64,    175, 2}, /* 1.0  [0] */
  {101,   1485,   99,     128,    350, 2}, /* 1.0b [1] */
  { 11,   3000,  396,     192,    500, 2}, /* 1.1  [2] */
  { 12,   6000,  396,     384,   1000, 2}, /* 1.2  [3] */
  { 13,  11880,  396,     768,   2000, 2}, /* 1.3  [4] */
  { 20,  11880,  396,    2000,   2000, 2}, /* 2.0  [5] */
  { 21,  19800,  792,    4000,   4000, 2}, /* 2.1  [6] */
  { 22,  20250, 1620,    4000,   4000, 2}, /* 2.2  [7] */
  { 30,  40500, 1620,   10000,  10000, 2}, /* 3.0  [8] */
  { 31, 108000, 3600,   14000,  14000, 4}, /* 3.1  [9] */
  { 32, 216000, 5120,   20000,  20000, 4}, /* 3.2  [10] */
  { 40, 245760, 8192,   20000,  25000, 4}, /* 4.0  [11] */
  { 41, 245760, 8192,   50000,  62500, 2}, /* 4.1  [12] */
  { 42, 522240, 8704,   50000,  62500, 2}, /* 4.2  [13] */
  { 50, 589824, 22080, 135000, 135000, 2}, /* 5.0  [14] */
  { 51, 983040, 36864, 240000, 240000, 2}  /* 5.1  [15] */
};


/*!
*************************************************************************************
* \brief
*    t_uint16 getIndexFromLevel (void)
*
* \note
*    This function returns the row index of LevelLimits table for current level
*
*************************************************************************************
*/
t_uint16 getIndexFromLevel (t_uint16 level)
{
  t_uint16 i;
  t_uint32 level_info = level;
  for (i=0; i < 16; i++) {
    if (level_info == LevelLimits[i][0]) {
      if (level_info != 11)
        break;
      else { /* NZ: here surely i>0 !!*/
        if (/*img->constraint_set3_flag*/ 0 == 0)
          break;
        else {
          i--;
          break;
        }
      }
    }
  }
  return i;
}

/*!
*************************************************************************************
* \brief
*    t_sint32 GenerateSeq_parameter_set_NALU ();
*
* \note
*    Uses the global variables through FillParameterSetStructures()
*
* \return
*    A NALU containing the Sequence ParameterSet
*
*************************************************************************************
*/
NALU_t *GenerateSeq_parameter_set_NALU (seq_parameter_set_rbsp_t *active_sps)
{
  NALU_t *n = AllocNALU(64000);
  t_sint32 RBSPlen = 0;

  byte *rbsp = (byte*)NULL;
  void * vfm_memory_ctxt = my_vfm_memory_ctxt;
/*  byte rbsp[MAXRBSPSIZE]; */
	rbsp = (byte *)malloc_(MAXRBSPSIZE * sizeof(char));
//   rbsp = (byte *)alloca(MAXRBSPSIZE * sizeof(char));

  RBSPlen = GenerateSeq_parameter_set_rbsp (active_sps, (char *)rbsp);
  RBSPtoNALU ((char *)rbsp, n, RBSPlen, NALU_TYPE_SPS, NALU_PRIORITY_HIGHEST, 0, 1);
  n->startcodeprefix_len = 4;

  free(rbsp);
  return n;
}

/*!
*************************************************************************************
* \brief
*    NALU_t *GeneratePic_parameter_set_NALU ();
*
* \note
*    Uses the global variables through FillParameterSetStructures()
*
* \return
*    A NALU containing the Picture Parameter Set
*
*************************************************************************************
*/
NALU_t *GeneratePic_parameter_set_NALU(pic_parameter_set_rbsp_t *active_pps)
{
  NALU_t *n = AllocNALU(64000);
  t_sint32 RBSPlen = 0;

  byte *rbsp = (byte*)NULL;
  void * vfm_memory_ctxt = my_vfm_memory_ctxt;
  /*byte rbsp[MAXRBSPSIZE]; */
  rbsp = (byte *)malloc_(MAXRBSPSIZE * sizeof(char));
//  rbsp = (byte *)alloca(MAXRBSPSIZE * sizeof(char));

  RBSPlen = GeneratePic_parameter_set_rbsp (active_pps, (char *)rbsp);
  RBSPtoNALU ((char *)rbsp, n, RBSPlen, NALU_TYPE_PPS, NALU_PRIORITY_HIGHEST, 0, 1);
  n->startcodeprefix_len = 4;
  free(rbsp);
  return n;
}

/*!
 ************************************************************************
 * \brief
 *    FillParameterSetStructures: extracts info from global variables and
 *    generates a picture and sequence parameter set structure
 *
 * \param sps
 *    Sequence parameter set to be filled
 * \param pps
 *    Picture parameter set to be filled
 * \par
 *    Function reads all kinds of values from several global variables,
 *    including input-> and image-> and fills in the sps and pps.  Many
 *    values are current hard-coded to defaults, especially most of the
 *    VUI stuff.  Currently, the sps and pps structures are fixed length
 *    This mode is not supported.  Hence, the function does not need to
 *    allocate memory for the FMOmap, the pointer slice_group_id is
 *    always NULL.
 *
 * \par
 *    Limitations
 *    Currently, the encoder does not support multiple parameter sets,
 *    primarily because the config file does not support it.  Hence the
 *    If one day multiple parameter sets are implemented, it would
 *    make sense to break this function into two, one for the picture and
 *    one for the sequence.
 *    The following pps and sps elements seem not to be used in the encoder
 *    or decoder and, hence, a guessed default value is conveyed:
 *
 *    pps->num_ref_idx_l1_active_minus1 = img->num_ref_pic_active_bwd_minus1;
 *    pps->chroma_qp_index_offset = 0;
 *    sps->required_frame_num_update_behaviour_flag = FALSE;
 *    sps->direct_temporal_constrained_flag = FALSE;
 *
 * \par
 *    Regarding the QP
 *    The previous software versions coded the absolute QP only in the
 *    slice header.  This is kept, and the offset in the PPS is coded
 *    even if we could save bits by intelligently using this field.
 *
 ************************************************************************
 */
t_sint32 profile_idc; /* HACK */
t_sint32 transform8x8; /* HACK */


#define Endianess(a)	(((((t_uint32)(a)) & 0xffff )<<16) + ((((t_uint32)(a)) >> 16) & 0xffff ))
void FillParameterSetStructures (seq_parameter_set_rbsp_t *sps,
                                 pic_parameter_set_rbsp_t *pps,
                                 int a_level_idc,
                                 t_specific_frameinfo* info,
                                 ts_t1xhv_vec_h264_param_in* h264_encode_par_in_nb)
{
  t_uint32 i;

  /* NZ */
/*  t_sint16 bit_rate_scale=0;
  t_uint32 bit_rate_value=0;
  t_uint16 cpb_size_scale=0;
  t_uint32 cpb_size_value=0;
  t_uint32 cpb_size;

  t_uint32 MaxCPB;*/
  /* NZ */

  /* ************************************************************************* */
  /* Sequence Parameter Set */
  /* ************************************************************************* */
  ASSERT (sps != NULL);
  ASSERT (pps != NULL);
  /* Profile and Level should be calculated using the info from the config */
  /* file.  Calculation is hidden in IndetifyProfile() and IdentifyLevel() */
//  sps->profile_idc = profile_idc = h264_encode_par_in_nb->ProfileIDC;
  sps->profile_idc = profile_idc = info->ProfileIDC;   /*change fo ndk5500_a0*/
  sps->level_idc = a_level_idc;

  /* needs to be set according to profile */
  sps->constrained_set0_flag = 0;
  sps->constrained_set1_flag = 0;
  sps->constrained_set2_flag = 0;
  sps->constrained_set3_flag = 0;

  /* Parameter Set ID hard coded to zero */
  sps->seq_parameter_set_id = 0;

  /* Fidelity Range Extensions stuff */
  sps->bit_depth_luma_minus8   = 0;
  sps->bit_depth_chroma_minus8 = 0;

  /*! POC stuff: */
  sps->log2_max_frame_num_minus4             = h264_encode_par_in_nb->log2_max_frame_num_minus4;
#ifdef __ndk8500_ed__
  sps->log2_max_pic_order_cnt_lsb_minus4     = h264_encode_par_in_nb->log2_max_pic_order_cnt_lsb_minus4;
#endif
  sps->pic_order_cnt_type                    = h264_encode_par_in_nb->pic_order_cnt_type;
  sps->num_ref_frames_in_pic_order_cnt_cycle = 1;
  sps->offset_for_non_ref_pic                = 0;

  for (i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
  {
    sps->offset_for_ref_frame[i] = 2;
  }
  /* End of POC stuff */


  /*required_frame_num_update_behaviour_flag hardcoded to zero */
  sps->gaps_in_frame_num_value_allowed_flag = FALSE;    /* double check */

  /* Picture size, finally a simple one :-) */
  sps->pic_width_in_mbs_minus1 = ((h264_encode_par_in_nb->window_width)/16) -1;
  sps->pic_height_in_map_units_minus1 = (((h264_encode_par_in_nb->window_height)/16)) - 1;

  /* Sequence VUI not implemented, signalled as not present */
  sps->vui_parameters_present_flag = 0;
  if (info->HrdSendMessages)
  {
    sps->vui_parameters_present_flag = 1;
    sps->vui_seq_parameters.aspect_ratio_info_present_flag = 1;
    sps->vui_seq_parameters.aspect_ratio_idc = 1;
    sps->vui_seq_parameters.overscan_info_present_flag = 0;
    sps->vui_seq_parameters.video_signal_type_present_flag = 1;
    sps->vui_seq_parameters.video_format = 5;
    sps->vui_seq_parameters.video_full_range_flag = info->video_full_range_flag; /* Default 1 */
    sps->vui_seq_parameters.colour_description_present_flag = 1;
    sps->vui_seq_parameters.colour_primaries = info->colour_primaries; /* Default 2 */
    sps->vui_seq_parameters.transfer_characteristics = 2;
    sps->vui_seq_parameters.matrix_coefficients = 2;
    sps->vui_seq_parameters.chroma_location_info_present_flag = 0;
    sps->vui_seq_parameters.timing_info_present_flag = 1;
    sps->vui_seq_parameters.num_units_in_tick = 1000;
#ifdef __ndk5500_a0__
    sps->vui_seq_parameters.time_scale = (((t_uint32)h264_encode_par_in_nb->framerate_num)*1000)/h264_encode_par_in_nb->framerate_den;
#else
    sps->vui_seq_parameters.time_scale = ((t_uint32)h264_encode_par_in_nb->framerate)*1000/1024;
#endif
    sps->vui_seq_parameters.fixed_frame_rate_flag = 0;
	if (info->HrdSendMessages == 1)
	{
      sps->vui_seq_parameters.nal_hrd_parameters_present_flag = 0;
      sps->vui_seq_parameters.vcl_hrd_parameters_present_flag = 0;
	}
	else
	{
	  //t_uint32 bit_rate_scale, bit_rate_value;
	  t_sint32 bit_rate_scale;
	  t_uint32 bit_rate_value;
	  t_sint32 cpb_size_scale;
	  t_uint32 cpb_size_value;
	  t_uint32 input_bit_rate;
	  t_uint32 cpb_buffer_size;

	  input_bit_rate = Endianess(h264_encode_par_in_nb->bit_rate);

      /* get bit_rate_scale and bit_rate_value from bitrate input from config file (BRC) */
      bit_rate_value = input_bit_rate;
	  bit_rate_scale =0;
      while (!(bit_rate_value & (t_uint32)0x01)) {
        bit_rate_value = bit_rate_value >> 1;
        bit_rate_scale++;
      }
      bit_rate_value--;
      bit_rate_scale -=6;

	  /* patch to avoid negs for bit_rate_scale */
	  if (bit_rate_scale<0) {
		  bit_rate_scale = 0;
		  bit_rate_value = (t_uint32) (input_bit_rate / 64) -1;
	  }

    /* NZ: patch for u_v() syntax element limit: it cannot write a more than 32 bit lenght bitpatter;
           Limiting the value to be u_v()-coded to 65534 the problem is avoided.
           This will limit the precision of the bitrate value coded in the stream */
    if (bit_rate_value > 65534) {
      while (bit_rate_value > 65534) {
        bit_rate_value /= 2;
        bit_rate_scale++;
      }
      bit_rate_value--; /* NZ: This "--" is to code a value minor than the wished one (ie to approximate it by defeats).
                             If you want to approximate it by excess you can safely comment this line */
    }
	  //cpb_buffer_size = 128000;
	  //cpb_buffer_size = Endianess(h264_encode_par_in_nb->bit_rate);
	  //+ code for CR 332873
	  cpb_buffer_size = Endianess(h264_encode_par_in_nb->CpbBufferSize);
	  //- code for CR 332873

      /* get cpb_size_scale and cpb_size_value from cpbsize input from config file (BRC) */
      cpb_size_value = cpb_buffer_size;
	  cpb_size_scale =0;
      while (!(cpb_size_value & (t_uint32)0x01)) {
        cpb_size_value = cpb_size_value >> 1;
        cpb_size_scale++;
      }
      cpb_size_value--;
      cpb_size_scale -=4;

	  /* patch to avoid negs for cpb_size_scale */
	  if (cpb_size_scale<0) {
		  cpb_size_scale = 0;
		  cpb_size_value = (t_uint32) (cpb_buffer_size / 16) -1;
	  }

    /* NZ: patch for u_v() syntax element limit: it cannot write a more than 32 bit lenght bitpatter;
           Limiting the value to be u_v()-coded to 65534 the problem is avoided.
           This will limit the precision of the cpbsize value coded in the stream */
    if (cpb_size_value > 65534) {
      while (cpb_size_value > 65534) {
        cpb_size_value /= 2;
        cpb_size_scale++;
      }
      cpb_size_value--; /* NZ: This "--" is to code a value minor than the wished one (ie to approximate it by defeats).
                             If you want to approximate it by excess you can safely comment this line */
    }
	  /* nal */
      sps->vui_seq_parameters.nal_hrd_parameters_present_flag = 1;
	  sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1 = 0;
	  sps->vui_seq_parameters.nal_hrd_parameters.bit_rate_scale = (t_uint32)bit_rate_scale;
	  sps->vui_seq_parameters.nal_hrd_parameters.cpb_size_scale = (t_uint32)cpb_size_scale;
	  sps->vui_seq_parameters.nal_hrd_parameters.bit_rate_value_minus1[0] = bit_rate_value;
	  sps->vui_seq_parameters.nal_hrd_parameters.cpb_size_value_minus1[0] = cpb_size_value;
	  sps->vui_seq_parameters.nal_hrd_parameters.cbr_flag[0] = 1;
      sps->vui_seq_parameters.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1 = 27;
	  sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 = 15;
      sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1 = 15;
	  sps->vui_seq_parameters.nal_hrd_parameters.time_offset_length = 0;
	  /* vcl */
      sps->vui_seq_parameters.vcl_hrd_parameters_present_flag = 1;
	  sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1 = 0;
	  sps->vui_seq_parameters.vcl_hrd_parameters.bit_rate_scale = (t_uint32)bit_rate_scale;
	  sps->vui_seq_parameters.vcl_hrd_parameters.cpb_size_scale = (t_uint32)cpb_size_scale;
	  sps->vui_seq_parameters.vcl_hrd_parameters.bit_rate_value_minus1[0] = bit_rate_value;
	  sps->vui_seq_parameters.vcl_hrd_parameters.cpb_size_value_minus1[0] = cpb_size_value;
	  sps->vui_seq_parameters.vcl_hrd_parameters.cbr_flag[0] = 1;
      sps->vui_seq_parameters.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1 = 27;
	  sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1 = 15;
      sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1 = 15;
	  sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length = 0;
      sps->vui_seq_parameters.low_delay_hrd_flag = 1;
	}
    sps->vui_seq_parameters.pic_struct_present_flag = 0;
    sps->vui_seq_parameters.bitstream_restriction_flag = 1;
    sps->vui_seq_parameters.motion_vectors_over_pic_boundaries_flag = 1;
    sps->vui_seq_parameters.max_bytes_per_pic_denom = 0;
    sps->vui_seq_parameters.max_bits_per_mb_denom = 0;
    sps->vui_seq_parameters.log2_max_mv_length_horizontal = 16;
    sps->vui_seq_parameters.log2_max_mv_length_vertical = 16;
    sps->vui_seq_parameters.num_reorder_frames = 0;
    sps->vui_seq_parameters.max_dec_frame_buffering = 1;
  }

  /* ************************************************************************* */
  /* Picture Parameter Set  */
  /* ************************************************************************* */

  pps->seq_parameter_set_id = 0;
  pps->entropy_coding_mode_flag = 0;
  for(i=0; i<8; i++)
    sps->seq_scaling_list_present_flag[i] = 0;

  pps->pic_scaling_matrix_present_flag = 0;
  for(i=0; i<8; i++)
    pps->pic_scaling_list_present_flag[i] = 0;

  /* JVT-Fxxx (by Stephan Wenger, make this flag unconditional */
  pps->pic_init_qs_minus26 = 0;
  pps->chroma_qp_index_offset = 0 /*input->chroma_qp_index_offset*/;      /* double check: is this chroma fidelity thing already implemented??? */
  pps->deblocking_filter_control_present_flag = 1; /* FP: parameters not sent */
  pps->constrained_intra_pred_flag = h264_encode_par_in_nb->use_constrained_intra_flag;
  pps->redundant_pic_cnt_present_flag = 0;

  sps->frame_cropping_flag               = FALSE;
  if (info->frame_cropping_flag)
  {
    sps->frame_cropping_flag               = TRUE;
    sps->frame_cropping_rect_left_offset   = info->frame_cropping_rect_left_offset;
    sps->frame_cropping_rect_right_offset  = info->frame_cropping_rect_right_offset;
    sps->frame_cropping_rect_top_offset    = info->frame_cropping_rect_top_offset;
    sps->frame_cropping_rect_bottom_offset = info->frame_cropping_rect_bottom_offset;
  }
  transform8x8 = h264_encode_par_in_nb->TransformMode;
}

/*!
 *************************************************************************************
 * \brief
 *    t_sint32 GenerateSeq_parameter_set_rbsp (seq_parameter_set_rbsp_t *sps, char *rbsp);
 *
 * \param sps
 *    sequence parameter structure
 * \param rbsp
 *    buffer to be filled with the rbsp, size should be at least MAXIMUMPARSETRBSPSIZE
 *
 * \return
 *    size of the RBSP in bytes
 *
 * \note
 *    Sequence Parameter VUI function is called, but the function implements
 *    an exit (-1)
 *************************************************************************************
 */
Bitstream bitstream_struct;
Bitstream bitstream_struct2;
Bitstream bitstream_struct3;
t_sint32 GenerateSeq_parameter_set_rbsp (seq_parameter_set_rbsp_t *sps, char *rbsp)
{
  Bitstream *bitstream = &bitstream_struct; /* FP */
  t_sint32 len = 0, LenInBytes;
  t_uint32 i;

  ASSERT (rbsp != NULL);
  /* In order to use the entropy coding functions from golomb.c we need  */
  /* to allocate a partition structure.  It will be freed later in this */
  /* function */
  //if ((bitstream=(Bitstream*)calloc(1, sizeof(Bitstream)))==NULL) no_mem_exit("SeqParameterSet:bitstream");

  /* .. and use the rbsp provided (or allocated above) for the data */
  bitstream->streamBuffer = (byte *)rbsp;
  bitstream->bits_to_go = 8;
  bitstream->byte_pos = 0;

  len+=u_v  (8,         sps->profile_idc,    bitstream);
  len+=u_1  ( sps->constrained_set0_flag,    bitstream);
  len+=u_1  ( sps->constrained_set1_flag,    bitstream);
  len+=u_1  ( sps->constrained_set2_flag,    bitstream);
  len+=u_1  ( sps->constrained_set3_flag,    bitstream);
  len+=u_v  (4,       0,                     bitstream);
  len+=u_v  (8,       sps->level_idc,        bitstream);
  len+=ue_v ( sps->seq_parameter_set_id,     bitstream);

  /* Fidelity Range Extensions stuff */
  if((sps->profile_idc==FREXT_HP) ||
     (sps->profile_idc==FREXT_Hi10P) ||
     (sps->profile_idc==FREXT_Hi422) ||
     (sps->profile_idc==FREXT_Hi444))
  {
    len+=ue_v ( 1,				bitstream);
    len+=ue_v ( sps->bit_depth_luma_minus8,                      bitstream);
    len+=ue_v ( sps->bit_depth_chroma_minus8,                    bitstream);
    len+=u_1  ( 0,                      bitstream);
    /*other chroma info to be added in the future */
    len+=u_1 ( 0,            bitstream);
  }

  len+=ue_v ( sps->log2_max_frame_num_minus4,                 bitstream);
  len+=ue_v ( sps->pic_order_cnt_type,                        bitstream);

  if (sps->pic_order_cnt_type == 0)
    len+=ue_v ( sps->log2_max_pic_order_cnt_lsb_minus4,         bitstream);
  else if (sps->pic_order_cnt_type == 1)
  {
    len+=u_1  ( DELTA_PIC_ORDER_ALWAYS_ZERO_FLAG,          bitstream);
    len+=se_v ( sps->offset_for_non_ref_pic,                    bitstream);
    len+=se_v ( 0,            bitstream);
    len+=ue_v ( sps->num_ref_frames_in_pic_order_cnt_cycle,     bitstream);
    for (i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
      len+=se_v (     sps->offset_for_ref_frame[i],                      bitstream);
  }
  len+=ue_v ( 1,                            bitstream);  /* FP: num_ref_frames forced to '1' */
  len+=u_1  ( sps->gaps_in_frame_num_value_allowed_flag,      bitstream);
  len+=ue_v ( sps->pic_width_in_mbs_minus1,                   bitstream);
  len+=ue_v ( sps->pic_height_in_map_units_minus1,            bitstream);
  len+=u_1  ( 1,				bitstream);
  len+=u_1  ( 1,				bitstream);
  len+=u_1  ( sps->frame_cropping_flag,                       bitstream);
  if (sps->frame_cropping_flag)
  {
    len+=ue_v ( sps->frame_cropping_rect_left_offset,           bitstream);
    len+=ue_v ( sps->frame_cropping_rect_right_offset,          bitstream);
    len+=ue_v ( sps->frame_cropping_rect_top_offset,            bitstream);
    len+=ue_v ( sps->frame_cropping_rect_bottom_offset,         bitstream);
  }

  len+=u_1  ( sps->vui_parameters_present_flag,               bitstream);
  if (sps->vui_parameters_present_flag)
    len+=GenerateVUISequenceParameters(sps, bitstream);    /* currently a dummy, asserting */

  SODBtoRBSP(bitstream);     /* copies the last couple of bits into the byte buffer */
  LenInBytes=bitstream->byte_pos;
  //free (bitstream);

  return LenInBytes;
}

/*!
 *************************************************************************************
 * \brief
 *    t_sint32 GeneratePic_parameter_set_rbsp (pic_parameter_set_rbsp_t *sps, char *rbsp);
 *
 * \param pps
 *    picture parameter structure
 * \param rbsp
 *    buffer to be filled with the rbsp, size should be at least MAXIMUMPARSETRBSPSIZE
 *
 * \return
 *    size of the RBSP in bytes, negative in case of an error
 *
 * \note
 *    Picture Parameter VUI function is called, but the function implements
 *    an exit (-1)
 *************************************************************************************
 */
t_sint32 GeneratePic_parameter_set_rbsp (pic_parameter_set_rbsp_t *pps, char *rbsp)
{
  Bitstream *bitstream = &bitstream_struct2; /* FP */
  t_sint32 len = 0, LenInBytes;
  //t_sint32 profile_idc;

  ASSERT (rbsp != NULL);

  /* In order to use the entropy coding functions from golomb.c we need  */
  /* to allocate a partition structure.  It will be freed later in this */
  /* function */
  //if ((bitstream=(Bitstream*)calloc(1, sizeof(Bitstream)))==NULL) no_mem_exit("PicParameterSet:bitstream");
  /* .. and use the rbsp provided (or allocated above) for the data */
  bitstream->streamBuffer = (byte *)rbsp;
  bitstream->bits_to_go = 8;
  bitstream->byte_pos = 0;

  len+=ue_v ( PIC_PARAMETER_SET_ID,                      bitstream);
  len+=ue_v ( pps->seq_parameter_set_id,                      bitstream);
  len+=u_1  ( pps->entropy_coding_mode_flag,                  bitstream);
  len+=u_1  ( PIC_ORDER_PRESENT_FLAG,                    bitstream);
  len+=ue_v ( 0,                   bitstream);
  len+=ue_v ( NUM_REF_IDX_L0_ACTIVE_MINUS1,              bitstream);

  /* FP: Warning, in the following: check which list to use (0 or 1)  */
  len+=ue_v ( NUM_REF_IDX_L0_ACTIVE_MINUS1,	      bitstream);

  len+=u_1  ( 0,					      bitstream);
  len+=u_v  ( 2,          0,					      bitstream);
  len+=se_v ( PIC_INIT_QP_MINUS26,                       bitstream);
  len+=se_v (      pps->pic_init_qs_minus26,                       bitstream);

  //profile_idc = PROFILE_IDC;
  if((profile_idc==FREXT_HP) ||
     (profile_idc==FREXT_Hi10P) ||
     (profile_idc==FREXT_Hi422) ||
     (profile_idc==FREXT_Hi444))
    len+=se_v (     0,   bitstream); /* FP: forced to zero */
  else
    len+=se_v (     pps->chroma_qp_index_offset,                    bitstream);

  len+=u_1  ( pps->deblocking_filter_control_present_flag,    bitstream);
  len+=u_1  ( pps->constrained_intra_pred_flag,               bitstream);
  len+=u_1  ( pps->redundant_pic_cnt_present_flag,            bitstream);

  if (profile_idc == FREXT_HP)
  {
    len+=u_1  ( transform8x8, bitstream); /* transform_8x8_mode_flag */
    len+=u_1  ( 0,            bitstream); /* pic_scaling_matrix_present_flag */
    len+=se_v ( 0,            bitstream); /* second_chroma_qp_index_offset */
  }

  SODBtoRBSP(bitstream);     /* copies the last couple of bits into the byte buffer */
  LenInBytes=bitstream->byte_pos;
  //free (bitstream);	/* Get rid of the helper structures */

  return LenInBytes;
}

/*!
 *************************************************************************************
 * \brief
 *    Returns max CPB size defined for given level
 *
 * \return
 *    Returns max CPB size defined for given level
*
 *************************************************************************************
 */
t_uint32 getMaxCPB(t_uint16 level)
{
  t_uint16 index = getIndexFromLevel(level);
  return LevelLimits[index][4];
}



/*!
 *************************************************************************************
 * \brief
 *    Function body for VUI Parameter generation (to be done)
 * \author Nicola Zandona'
 * \return
 *    exits with error message
 *************************************************************************************
 */
static t_sint32 GenerateVUISequenceParameters(seq_parameter_set_rbsp_t *sps, Bitstream *bitstream)
{
    t_sint32 len=0;
    t_uint32 i;

    len+=u_1  ( sps->vui_seq_parameters.aspect_ratio_info_present_flag, bitstream);
    if (sps->vui_seq_parameters.aspect_ratio_info_present_flag)
    {
        len+=u_v  (8, sps->vui_seq_parameters.aspect_ratio_idc, bitstream);
        if (sps->vui_seq_parameters.aspect_ratio_idc == 255)
        {
            len+=u_v  (16, sps->vui_seq_parameters.sar_width, bitstream);
            len+=u_v  (16, sps->vui_seq_parameters.sar_height, bitstream);
        }
    }

    len+=u_1  ( sps->vui_seq_parameters.overscan_info_present_flag, bitstream);
    if (sps->vui_seq_parameters.overscan_info_present_flag)
        len+=u_1  ( sps->vui_seq_parameters.overscan_appropriate_flag, bitstream);

    len+=u_1  ( sps->vui_seq_parameters.video_signal_type_present_flag, bitstream);
    if (sps->vui_seq_parameters.video_signal_type_present_flag)
    {
        len+=u_v  (3, sps->vui_seq_parameters.video_format, bitstream);
        len+=u_1  ( sps->vui_seq_parameters.video_full_range_flag, bitstream);
        len+=u_1  ( sps->vui_seq_parameters.colour_description_present_flag, bitstream);
        if (sps->vui_seq_parameters.colour_description_present_flag)
        {
            len+=u_v  (8, sps->vui_seq_parameters.colour_primaries, bitstream);
            len+=u_v  (8, sps->vui_seq_parameters.transfer_characteristics, bitstream);
            len+=u_v  (8, sps->vui_seq_parameters.matrix_coefficients, bitstream);
        }
    }

    len+=u_1  ( sps->vui_seq_parameters.chroma_location_info_present_flag, bitstream);
    if (sps->vui_seq_parameters.chroma_location_info_present_flag)
    {
        len+=ue_v ( sps->vui_seq_parameters.chroma_sample_loc_type_top_field, bitstream);
        len+=ue_v ( sps->vui_seq_parameters.chroma_sample_loc_type_bottom_field, bitstream);
    }

    len+=u_1  ( sps->vui_seq_parameters.timing_info_present_flag, bitstream);
    if (sps->vui_seq_parameters.timing_info_present_flag)
    {
        len+=u_v  (32, sps->vui_seq_parameters.num_units_in_tick, bitstream);
        len+=u_v  (32, sps->vui_seq_parameters.time_scale, bitstream);
        len+=u_1  ( sps->vui_seq_parameters.fixed_frame_rate_flag, bitstream);
    }

    len+=u_1  (sps->vui_seq_parameters.nal_hrd_parameters_present_flag, bitstream);
    if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
    {
        len+=ue_v ( sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1, bitstream);
        len+=u_v  (4, sps->vui_seq_parameters.nal_hrd_parameters.bit_rate_scale, bitstream);
        len+=u_v  (4, sps->vui_seq_parameters.nal_hrd_parameters.cpb_size_scale, bitstream);
        for (i=0; i < sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1 +1; i++)
        {
            len+=ue_v  ( sps->vui_seq_parameters.nal_hrd_parameters.bit_rate_value_minus1[i], bitstream);
            len+=ue_v  ( sps->vui_seq_parameters.nal_hrd_parameters.cpb_size_value_minus1[i], bitstream);
            len+=u_1  ( sps->vui_seq_parameters.nal_hrd_parameters.cbr_flag[i], bitstream);
        }
        len+=u_v  (5, sps->vui_seq_parameters.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1, bitstream);
        len+=u_v  (5, sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1, bitstream);
        len+=u_v  (5, sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1, bitstream);
        len+=u_v  (5, sps->vui_seq_parameters.nal_hrd_parameters.time_offset_length, bitstream);
    }

    len+=u_1  ( sps->vui_seq_parameters.vcl_hrd_parameters_present_flag, bitstream);
    if (sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
    {
        len+=ue_v  ( sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1, bitstream);
        len+=u_v  (4, sps->vui_seq_parameters.vcl_hrd_parameters.bit_rate_scale, bitstream);
        len+=u_v  (4, sps->vui_seq_parameters.vcl_hrd_parameters.cpb_size_scale, bitstream);
        for (i=0; i < sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1 +1; i++)
        {
            len+=ue_v  ( sps->vui_seq_parameters.vcl_hrd_parameters.bit_rate_value_minus1[i], bitstream);
            len+=ue_v  ( sps->vui_seq_parameters.vcl_hrd_parameters.cpb_size_value_minus1[i], bitstream);
            len+=u_1  ( sps->vui_seq_parameters.vcl_hrd_parameters.cbr_flag[i], bitstream);
        }
        len+=u_v  (5, sps->vui_seq_parameters.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1, bitstream);
        len+=u_v  (5, sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1, bitstream);
        len+=u_v  (5, sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1, bitstream);
        len+=u_v  (5, sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length, bitstream);
    }

    if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag || sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
        len+=u_1  ( sps->vui_seq_parameters.low_delay_hrd_flag, bitstream);

    len+=u_1  ( sps->vui_seq_parameters.pic_struct_present_flag, bitstream);

    len+=u_1  ( sps->vui_seq_parameters.bitstream_restriction_flag, bitstream);
    if (sps->vui_seq_parameters.bitstream_restriction_flag)
    {
        len+=u_1  ( sps->vui_seq_parameters.motion_vectors_over_pic_boundaries_flag, bitstream);
        len+=ue_v ( sps->vui_seq_parameters.max_bytes_per_pic_denom, bitstream);
        len+=ue_v ( sps->vui_seq_parameters.max_bits_per_mb_denom, bitstream);
        len+=ue_v ( sps->vui_seq_parameters.log2_max_mv_length_horizontal, bitstream);
        len+=ue_v ( sps->vui_seq_parameters.log2_max_mv_length_vertical, bitstream);
        len+=ue_v ( sps->vui_seq_parameters.num_reorder_frames, bitstream);
        len+=ue_v ( sps->vui_seq_parameters.max_dec_frame_buffering, bitstream);
    }
    return len;
}

static t_sint32 GenerateSEI_rbsp(char* rbsp, int buf_period, int pic_num)
{
	Bitstream *bitstream = &bitstream_struct3; /* FP */
    t_sint32 len = 0, LenInBytes;;

  ASSERT (rbsp != NULL);

  bitstream->streamBuffer = (byte *)rbsp;
  bitstream->bits_to_go = 8;
  bitstream->byte_pos = 0;

	/* Buffering period */
  if (buf_period)
  {
    len += u_v(8, 0, bitstream);
    len += u_v(8, 15, bitstream);
	len += ue_v(0, bitstream);
    len += u_v(28, 6000, bitstream);
    len += u_v(28, 0x0AAAAAAA, bitstream);
    len += u_v(28, 6000, bitstream);
    len += u_v(28, 0x0AAAAAAA, bitstream);
    len += u_v(1, 1, bitstream);
    len += u_v(6, 0, bitstream);
  }
     /* Pic timing */
    len += u_v(8, 1, bitstream);
    len += u_v(8, 4, bitstream);
    len += u_v(16, pic_num ? pic_num : 0xAAAA, bitstream);
    len += u_v(16, pic_num ? pic_num : 0xAAAA, bitstream);

    /*len += u_v(1, 1, bitstream);
    len += u_v(7, 0, bitstream);*/

	SODBtoRBSP(bitstream);     /* copies the last couple of bits into the byte buffer */
  LenInBytes=bitstream->byte_pos;
  //free (bitstream);	/* Get rid of the helper structures */

	return LenInBytes;
}

NALU_t* GenerateSEI_NALU_(int buf_period, int pic_num)
{
  NALU_t *n = AllocNALU(64000);
  t_sint32 RBSPlen = 0;

  byte *rbsp = (byte*)NULL;
  void * vfm_memory_ctxt = my_vfm_memory_ctxt;
  rbsp = (byte *)malloc_(MAXRBSPSIZE * sizeof(char));

  RBSPlen = GenerateSEI_rbsp((char *)rbsp, buf_period, pic_num);
  RBSPtoNALU ((char *)rbsp, n, RBSPlen, NALU_TYPE_SEI, NALU_PRIORITY_DISPOSABLE, 0, 1);
  n->startcodeprefix_len = 4;
  free(rbsp);
  return n;
}

/*!
 *************************************************************************************
 * \brief
 *    Allocates memory for a NALU
 *
 * \param buffersize
 *     size of NALU buffer
 *
 * \return
 *    pointer to a NALU
 *************************************************************************************
 */
NALU_t nalu;
t_uint8* bitbuf;

NALU_t *AllocNALU(t_uint32 buffersize)
{
  NALU_t *n = &nalu;

  /* Olivier : Modif alloc method */
//  if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL) no_mem_exit ("AllocNALU: n");
  //if ((n = (NALU_t*)VAL_AllocDdramBuffer(sizeof (NALU_t), 1, ALLOC_IN_ANY_DDRAM_BANK)) == NULL) no_mem_exit ("AllocNALU: n");

  n->max_size=buffersize;
  n->buf = (byte*)bitbuf;

//  if ((n->buf = (byte*)calloc (buffersize, sizeof (byte))) == NULL) no_mem_exit ("AllocNALU: n->buf");
  //if ((n->buf = (byte*)VAL_AllocDdramBuffer(sizeof (byte) * buffersize, 1, ALLOC_IN_ANY_DDRAM_BANK)) == NULL) no_mem_exit ("AllocNALU: n->buf");

  return n;
}


/*!
 *************************************************************************************
 * \brief
 *    Frees a NALU
 *
 * \param n
 *    NALU to be freed
 *
 *************************************************************************************
 */
#if 0
void FreeNALU(NALU_t *n)
{
  if (n)
  {
    if (n->buf)
    {
      free(n->buf);
      n->buf=NULL;
    }
    free (n);
  }
}
#endif

/*!
************************************************************************
*  \brief
*     This function converts a RBSP payload to an EBSP payload
*
*  \param streamBuffer
*       pointer to data bits
*  \param begin_bytepos
*            The byte position after start-code, after which stuffing to
*            prevent start-code emulation begins.
*  \param end_bytepos
*           Size of streamBuffer in bytes.
*  \param min_num_bytes
*           Minimum number of bytes in payload. Should be 0 for VLC entropy
*           coding mode. Determines number of stuffed words for CABAC mode.
*  \return
*           Size of streamBuffer after stuffing.
*  \note
*      NAL_Payload_buffer is used as temporary buffer to store data.
*
*
************************************************************************
*/
#define NAL_Payload_buffer streamBuffer   /* Olivier: Dont need a table of MAX_STREAMBUF_SIZE */
t_sint32 RBSPtoEBSP(byte *streamBuffer, t_sint32 begin_bytepos, t_sint32 end_bytepos, t_sint32 min_num_bytes)
{

  t_sint32 i, j, count;

  /* Olivier remove */
/*  static byte NAL_Payload_buffer[MAX_STREAMBUF_SIZE];

  for(i = begin_bytepos; i < end_bytepos; i++)
    NAL_Payload_buffer[i] = streamBuffer[i];
*/
  count = 0;
  j = begin_bytepos;
  for(i = begin_bytepos; i < end_bytepos; i++)
  {
    if(count == ZEROBYTES_SHORTSTARTCODE && !(NAL_Payload_buffer[i] & 0xFC))
    {
      streamBuffer[j] = 0x03;
      j++;
      count = 0;
    }

    streamBuffer[j] = NAL_Payload_buffer[i];
    if(NAL_Payload_buffer[i] == 0x00)
      count++;
    else
      count = 0;
    j++;
  }
  while (j < begin_bytepos+min_num_bytes) {
    streamBuffer[j]   = 0x00; /* cabac stuffing word */
    streamBuffer[j+1] = 0x00;
    streamBuffer[j+2] = 0x03;
    j += 3;
	 /* We Did not use Stat functions */
    /* stats->bit_use_stuffingBits[h264_encode_par_in_nb[0]->picture_coding_type]+=16; */
  }
  return j;
}

 /*!
 ************************************************************************
 * \brief
 *    Converts String Of Data Bits (SODB) to Raw Byte Sequence
 *    Packet (RBSP)
 * \param currStream
 *        Bitstream which contains data bits.
 * \return None
 * \note currStream is byte-aligned at the end of this function
 *
 ************************************************************************
*/
void SODBtoRBSP(Bitstream *currStream)
{
  currStream->byte_buf <<= 1;
  currStream->byte_buf |= 1;
  currStream->bits_to_go--;
  currStream->byte_buf <<= currStream->bits_to_go;
  currStream->streamBuffer[currStream->byte_pos++] = currStream->byte_buf;
  currStream->bits_to_go = 8;
  currStream->byte_buf = 0;
}

/*!
 *************************************************************************************
 * \brief
 *    Converts an RBSP to a NALU
 *
 * \param rbsp
 *    byte buffer with the rbsp
 * \param nalu
 *    nalu structure to be filled
 * \param rbsp_size
 *    size of the rbsp in bytes
 * \param nal_unit_type
 *    as in JVT doc
 * \param nal_reference_idc
 *    as in JVT doc
 * \param min_num_bytes
 *    some incomprehensible CABAC stuff
 * \param UseAnnexbLongStartcode
 *    when 1 and when using AnnexB bytestreams, then use a long startcode prefix
 *
 * \return
 *    length of the NALU in bytes
 *************************************************************************************
 */
t_sint32 RBSPtoNALU (char *rbsp, NALU_t *nalu, t_sint32 rbsp_size, t_sint32 nal_unit_type, t_sint32 nal_reference_idc,
                t_sint32 min_num_bytes, t_sint32 UseAnnexbLongStartcode)
{
  t_sint32 len;

  ASSERT (nalu != NULL);
  ASSERT (nal_reference_idc <=3 && nal_reference_idc >=0);
  ASSERT (nal_unit_type > 0 && nal_unit_type <= 10);
  ASSERT (rbsp_size < MAXRBSPSIZE);

  nalu->forbidden_bit = 0;
  nalu->nal_reference_idc = nal_reference_idc;
  nalu->nal_unit_type = nal_unit_type;
  nalu->startcodeprefix_len = UseAnnexbLongStartcode?4:3;
  nalu->buf[0] =
    nalu->forbidden_bit << 7      |
    nalu->nal_reference_idc << 5  |
    nalu->nal_unit_type;

  //memcpy (& (nalu->buf[1]), rbsp, rbsp_size);
  for (int i = 0; i < rbsp_size; i++)
    nalu->buf[1+i] = *(rbsp+i);

/* printf ("First Byte %x\n", nalu->buf[0]); */
/* printf ("RBSPtoNALU: Before: NALU len %d\t RBSP %x %x %x %x\n", rbsp_size, (t_uint32) nalu->buf[1], (t_uint32) nalu->buf[2], (t_uint32) nalu->buf[3], (t_uint32) nalu->buf[4]); */

  len = 1 + RBSPtoEBSP (& (nalu->buf[1]), 0, rbsp_size, min_num_bytes);

/* printf ("RBSPtoNALU: After : NALU len %d\t EBSP %x %x %x %x\n", rbsp_size, (t_uint32) nalu->buf[1], (t_uint32) nalu->buf[2], (t_uint32) nalu->buf[3], (t_uint32) nalu->buf[4]); */
/* printf ("len %d\n\n", len); */
  nalu->len = len;

  return len;
}

/*!
 ************************************************************************
 * \brief
 *    Exit program if memory allocation failed (using error())
 * \param where
 *    string indicating which memory allocation failed
 ************************************************************************
 */
void no_mem_exit(char *where)
{
   /*snprintf(errortext, ET_SIZE, "Could not allocate memory: %s",where);
   error (errortext, 100);*/

	//printf("Could not allocate memory: %s",where);
}

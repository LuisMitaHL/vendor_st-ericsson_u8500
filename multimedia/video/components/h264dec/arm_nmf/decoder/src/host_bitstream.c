/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef NMF_BUILD
#include <h264dec/arm_nmf/decoder.nmf>
#endif

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_arm_nmf_decoder_src_host_bitstreamTraces.h"
#endif

#include "types.h"
#include "host_types.h"
#include "macros.h"
#include "settings.h"
#include "host_bitstream.h"
#include "host_sh_concealment.h"
#include "common_bitstream.h"
#include "host_DPB.h"

#if ENABLE_VUI == 1
    #include "host_vui.h"
#endif



/* Local functions prototypes */
t_uint16 CheckFrameNum(t_uint32 frame_num, t_uint16 nri, t_uint16 first_slice, t_old_slice *old_sl, t_seq_par *p_sp, t_dec_buff *buf);
t_uint16 MoreRbspData(t_bit_buffer *p_b);
t_uint16 ScalingList(t_bit_buffer *p_b, t_uint16 slist_index, tps_ScalingList p_pic_scaling_list, te_ScalingListType sl_kind);
t_uint16 pred_weight_table1(t_bit_buffer *p_b,t_slice_hdr *p_s);
t_uint16 SetDefault_ScalingList(t_uint16 slist_index,
  tps_ScalingList p_scaling_list, te_ScalingListType sl_kind);
t_uint16 SetDefault_ScalingList_setA(t_uint16 slist_index,
  tps_ScalingList p_scaling_list, te_ScalingListType sl_kind);
t_uint16 SetDefault_ScalingList_setB(t_uint16 slist_index,
  tps_ScalingList p_scaling_list, te_ScalingListType sl_kind, t_uint16 sps_id, t_seq_par * p_s, t_sint16*p_nsp);

void METH(CheckLevelVsFrameSize)(t_seq_par *p_s)
{
  t_uint32 frame_size_in_mbs = (p_s->pic_width_in_mbs_minus1 +1) * (p_s->pic_height_in_map_units_minus1+1)
                               * (2-p_s->frame_mbs_only_flag);
  t_uint16 level = p_s->level_idc;
  t_uint16 max_frame_size=0;

  if((frame_size_in_mbs > 36864)||(level<9)||(level>51))
   return;

  do
  {
    if(level<11)
    {
      max_frame_size = 99;
    }else if(level<21)
    {
      max_frame_size = 396;
    }else if(level<22)
    {
      max_frame_size = 792;
    }else if(level<31)
    {
      max_frame_size = 1620;
    }else if(level<32)
    {
      max_frame_size = 3600;
    }else if(level<40)
    {
      max_frame_size = 5120;
    }else if(level<42)
    {
      max_frame_size = 8192;
    }else if(level<50)
    {
      max_frame_size = 8704;
    }else if(level<51)
    {
      max_frame_size = 22080;
    }else
    {
      max_frame_size = 36864;
    }

    if(max_frame_size < frame_size_in_mbs)
      level++;
  }while(max_frame_size < frame_size_in_mbs);

  if(level!=p_s->level_idc)
  {
    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: stream's level (%d) is not in line with frame size, changing level to %d\n",p_s->level_idc,level);
    p_s->level_idc = level;
  }
}



/**
 * \brief Retrieves the NALU type.
 * \param p_b Address of the pointer to the bit_buffer struct
 * \param p_nal_ref_idc Pointer to an integer where the Reference Indication flag will be written
 * \param p_nal_unit_type Pointer to an integer that will contain the NALU type
 * \return 0 if successful, 1 otherwise
 * \author Filippo Santinello
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>23-06-2004&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function looks for the nearest NALU in scan order, and synchronizes the bitstream
 * handling parameters to it, if any is found. The function also looks for the start position
 * of the subsequent NALU, if any, and returns its position in a separate pointer.
 */

t_uint16 METH(GetNALUType)(t_bit_buffer *p_b, t_uint16 *p_nal_ref_idc, t_uint16 *p_nal_unit_type, t_uint16 is_mvc_stream)
{
    t_uint16 status;
    t_uint32 value;

    status = ShowBits(p_b, 8, &value);
    if (status!=0)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Abnormal bitstream termination. Exiting\n");
        return 1;
    }
    (void)FlushBits(p_b,8);

    *p_nal_ref_idc = (value>>5) & 0x3;
    *p_nal_unit_type = value & 0x1f;

    /* Check baseline constraints */
    /* ignore SVC NALs */
    if (
        ((*p_nal_unit_type >= 2) && (*p_nal_unit_type <= 4))   ||
        ((*p_nal_unit_type == 14) && (is_mvc_stream ==0))      ||
        ((*p_nal_unit_type == 20) && (is_mvc_stream ==0))
       )
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Invalid or unsupported NAL Unit type %d. Skipping\n",*p_nal_unit_type);
        return 2;
    }

    /* Check forbidden_zero_bit */
    if ((value>>7) != 0)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: WARNING! forbidden_zero_bit should be 0 in NAL Unit\n");
    }

    /* Display NAL Unit reference indication and type */
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: nal_ref_idc = %d\n",*p_nal_ref_idc);
    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: nal_unit_type = ");

    switch(*p_nal_unit_type)
    {
        case 0:
        OstTraceFiltInst0( TRACE_DEBUG , "UNSPECIFIED\n");
        break;

        case 1:
        OstTraceFiltInst0( TRACE_DEBUG , "Coded Slice, non-IDR picture\n");
        break;

        case 2:
        OstTraceFiltInst0( TRACE_DEBUG , "Coded Slice, Data Partition A\n");
        break;

        case 3:
        OstTraceFiltInst0( TRACE_DEBUG , "Coded Slice, Data Partition B\n");
        break;

        case 4:
        OstTraceFiltInst0( TRACE_DEBUG , "Coded Slice, Data Partition C\n");
        break;

        case 5:
        OstTraceFiltInst0( TRACE_DEBUG , "Coded Slice, IDR picture\n");
        break;

        case 6:
        OstTraceFiltInst0( TRACE_DEBUG , "Supplemental Enhancement Information\n");
        break;

        case 7:
        OstTraceFiltInst0( TRACE_DEBUG , "Sequence Parameters Set\n");
        break;

        case 8:
        OstTraceFiltInst0( TRACE_DEBUG , "Picture Parameters Set\n");
        break;

        case 9:
        OstTraceFiltInst0( TRACE_DEBUG , "Access Unit Delimiter\n");
        break;

        case 10:
        OstTraceFiltInst0( TRACE_DEBUG , "End Of Sequence\n");
        break;

        case 11:
        OstTraceFiltInst0( TRACE_DEBUG , "End Of Stream\n");
        break;

        case 12:
        OstTraceFiltInst0( TRACE_DEBUG , "Filler Data\n");
        break;

        case 14:
        OstTraceFiltInst0( TRACE_DEBUG , "Prefix\n");
        break;

        case 15:
        OstTraceFiltInst0( TRACE_DEBUG , "Subset Sequence Parameter Set\n");
        break;

        case 20:
        OstTraceFiltInst0( TRACE_DEBUG , "Coded Slice Extension\n");
        break;

        case 24:
        OstTraceFiltInst0( TRACE_DEBUG , "View and Dependency Representation Delimiter\n");
        break;

        default:
        OstTraceFiltInst0( TRACE_DEBUG , "RESERVED or UNSPECIFIED\n");
        break;
    }

    return 0;
}



/**
 * \brief Reads a Sequence Parameters Set NALU.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_nsp Pointer to variable holding the number of available sequence parameter sets
 * \param p_s Array of sequence paramet set structures
 * \return 0 if successful, 1 otherwise
 * \author Denis Zinato
 * \author Filippo Santinello
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>28-06-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>Added error check.</td></tr>
 * <tr><td valign=top>20-02-2006&nbsp;</td><td>Added sps update support.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a Sequence Parameters Set NALU.
 */

t_uint16 METH(GetSequenceParSet)(t_bit_buffer *p_b, t_sint16 *p_nsp, t_seq_par *p_s, t_seq_par *tmp_sps, t_uint16 * pending_sps_update)
{
    t_uint32 value;
    t_sint32 svalue;
    t_uint16 status, i;
    t_uint16 tmp_profile_idc, tmp_constraint_flags, tmp_level_idc;
    t_sint16 index;

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: ---- SEQUENCE PARAMETER SET ----\n");

    /* Profile indication */
    status = ShowBits(p_b,8,&value);
    if (status!=0)
        return 1;
    tmp_profile_idc = (t_uint16)value;
    (void)FlushBits(p_b,8);

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: profile_idc =%u\n",tmp_profile_idc);

    /* Constraint flags and reserved zero bits */
    status = ShowBits(p_b,8,&value);
    if (status!=0)
        return 1;

    if ((value&0x3)!=0)
    {
       OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Warning! reserved_zero_2bits in Sequence Parameter Set should be 0\n");
    }

    tmp_constraint_flags = (t_uint16)(value >> 2);
    (void)FlushBits(p_b,8);

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: constraint_set0_flag =%u\n",(tmp_constraint_flags>>5));
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: constraint_set1_flag =%u\n",((tmp_constraint_flags>>4)&0x1));
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: constraint_set2_flag =%u\n",((tmp_constraint_flags>>3)&0x1));
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: constraint_set3_flag =%u\n",((tmp_constraint_flags>>2)&0x1));
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: constraint_set4_flag =%u\n",((tmp_constraint_flags>>1)&0x1));
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: constraint_set5_flag =%u\n",(tmp_constraint_flags&0x1));

    /* Check profile */
    if (
         ( (tmp_profile_idc != 128) ) &
         ( (tmp_profile_idc != 118) ) &
         ( (tmp_profile_idc != 100) ) &
         ( (tmp_profile_idc != 88)  ) &
         ( (tmp_profile_idc != 77)  ) &
         ( (tmp_profile_idc != 66) && ((tmp_constraint_flags>>5) != 1) )
       )
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: The bitstream does not comply with Baseline/Main/High constraints.\n");
        return 1;
    }
    /* Pure Extended Profile (profile_idc=88 and neither constraint_set0_flag=1, nor constraint_set1_flag=1) is not supported */
    else if ( (88 == tmp_profile_idc) & (0x0 == (tmp_constraint_flags & 0x30)) )
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: The bitstream does not comply with Baseline/Main/High constraints.\n");
        return 1;
    }

    /* Level indication */
    status = ShowBits(p_b,8,&value);
    if ((status!=0) || CheckLevel(value,tmp_profile_idc))
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid level %i\n", (t_uint16)value);
        return 1;
    }

    tmp_level_idc = (t_uint16)value;
    (void)FlushBits(p_b,8);

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: level_idc =%u\n",tmp_level_idc);

    /* Sequence parameter set ID */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
    if ((status!=0) || (svalue > 31))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid sequence_parameter_set_id\n");
        return 1;
    }

    /* Check if this sequence parameter set is an update */
    i = 0;
    while ( (i < *p_nsp) && (p_s->seq_par_set_id != (t_uint16)svalue) )
    {
        i++;
        p_s++;
    }
    if (i == *p_nsp)
    {
        (*p_nsp)++;
    }
    else
    {
      /* Special case of a new SPS with the same ID.
         This can correspond to two situations:
         (a) it is just a repetition of existing one, no problem
         (b) it is a new sequence, with different parameters
         In case (b) we cannot simply smash the entry in SPS table,
         because SPS is currently active and in use and its values
         can be read after the parsing of the new SPS (due to pipe effect),
         so we write it into a temporary location and put it into the table
         later, when it is activated.
        */
      p_s = tmp_sps;
      *pending_sps_update = 1;
      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: WARNING: Duplicate seq_parameter_set_id = %u, doing backup\n",svalue);
    }

    p_s->profile_idc = tmp_profile_idc;
    p_s->constraint_flags = tmp_constraint_flags;
    p_s->level_idc = tmp_level_idc;

    p_s->seq_par_set_id = (t_uint16)svalue;

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: seq_parameter_set_id =%u\n",p_s->seq_par_set_id);

    /* High Profile or MVC */
    if ( ( p_s->profile_idc == 100 ) || ( p_s->profile_idc == 118 ) || ( p_s->profile_idc == 128 ) )
    {
      /* chroma_format_idc : should be always = 1 in High Profile 420 (no FrExt)*/
      status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
      if ((status!=0) || (svalue != 1))
      {
          OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid chroma_format_idc %ld (HIGH PROFILE only: No FrExt)\n",svalue);
          return 1;
      }
      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: chroma_format_idc =%ld\n",svalue);

      /* bit_depth_luma_minus8  : should be always = 0 in High Profile 420 (no FrExt)*/
      status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
      if ((status!=0) || (svalue != 0))
      {
          OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid bit_depth_luma_minus8 %ld (HIGH PROFILE only: No FrExt)\n",svalue);
          return 1;
      }
      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: bit_depth_luma_minus8 =%ld\n",svalue);

      /* bit_depth_chroma_minus8  : should be always = 0 in High Profile 420 (no FrExt)*/
      status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
      if ((status!=0) || (svalue != 0))
      {
          OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid bit_depth_chroma_minus8 %ld (HIGH PROFILE only: No FrExt)\n",svalue);
          return 1;
      }
      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: bit_depth_chroma_minus8 =%ld\n",svalue);

      /* qpprime_y_zero_transform_bypass_flag  : should be always = 0 in High Profile 420 (no FrExt)*/
      status = ShowBits(p_b,1,&value);
      if ((status!=0) || (value != 0))
      {
          OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid qpprime_y_zero_transform_bypass_flag %ld (HIGH PROFILE only: No FrExt)\n",value);
          return 1;
      }
      (void)FlushBits(p_b,1);
      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: qpprime_y_zero_transform_bypass_flag =%ld\n",value);

      /* seq_scaling_matrix_present_flag */
      status = ShowBits(p_b,1,&value);
      if ((status!=0) || (value > 1) )
      {
          OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid seq_scaling_matrix_present_flag \n");
          return 1;
      }
      p_s->seq_scaling_matrix_present_flag = (t_uint16 )value;
      (void)FlushBits(p_b,1);
      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: seq_scaling_matrix_present_flag =%u\n",p_s->seq_scaling_matrix_present_flag);

      if (p_s->seq_scaling_matrix_present_flag == 1)
      {
        for (i=0; i<8 ; i++)
        {
          /* reading seq_scaling_list_present_flag[i] */
          status = ShowBits(p_b,1,&value);
          if ((status!=0) || (value > 1) )
          {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid seq_scaling_list_present_flag[%d] \n",i);
            return 1;
          }
          p_s->seq_scaling_list_present_flag[i] = (t_uint16 )value;
          (void)FlushBits(p_b,1);
          OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: seq_scaling_list_present_flag[%d] =%u\n",i,p_s->seq_scaling_list_present_flag[i]);
          /* end reading seq_scaling_list_present_flag[i] */

          if ( p_s->seq_scaling_list_present_flag[i] == 1 )
          {
            if (i<6)
            {
              status = ScalingList(p_b, i  , &p_s->seq_scaling_list, SCALING_LIST_4x4);
              if(p_s->seq_scaling_list.UseDefaultScalingMatrix4x4Flag[i])
                SetDefault_ScalingList(i,&p_s->seq_scaling_list,SCALING_LIST_4x4);
            }
            else
            {
              status = ScalingList(p_b, i-6, &p_s->seq_scaling_list, SCALING_LIST_8x8);
              if(p_s->seq_scaling_list.UseDefaultScalingMatrix8x8Flag[i-6])
                SetDefault_ScalingList(i-6,&p_s->seq_scaling_list,SCALING_LIST_8x8);
            }
            if (status)
              return 1;

          }
          else
          {
            if(i<6)
              SetDefault_ScalingList_setA(i,&p_s->seq_scaling_list,SCALING_LIST_4x4);
            else
              SetDefault_ScalingList_setA(i-6,&p_s->seq_scaling_list,SCALING_LIST_8x8);
          }

        }
      }

    }
    else /* INFERRED VALUES in case profile is != from HIGH */
    {
      p_s->seq_scaling_matrix_present_flag = 0; /* Always in Main and Baseline */

    }

    /* Max frame number */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

    if ((status!=0) || (svalue > 12))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid log2_max_frame_num_minus4\n");
        return 1;
    }

    p_s->log2_max_frame_num_minus4 = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: log2_max_frame_num_minus4 =%u\n",p_s->log2_max_frame_num_minus4);

    /* Picture order count type */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

    if ((status!=0) || (svalue > 2))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid pic_order_cnt_type\n");
        return 1;
    }

    p_s->pic_order_cnt_type = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_order_cnt_type =%u\n",p_s->pic_order_cnt_type);

    p_s->delta_pic_order_always_zero_flag = 0;

    if (p_s->pic_order_cnt_type == 0)
    {
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

        if ((status!=0) || (svalue > 12))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid log2_max_pic_order_cnt_lsb_minus4\n");
            return 1;
        }

        p_s->log2_max_pic_order_cnt_lsb_minus4 = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: log2_max_pic_order_cnt_lsb_minus4 =%u\n",p_s->log2_max_pic_order_cnt_lsb_minus4);
    }
    else if (p_s->pic_order_cnt_type == 1)
    {
        status = ShowBits(p_b,1,&value);
        if (status!=0)
            return 1;
        p_s->delta_pic_order_always_zero_flag = value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: delta_pic_order_always_zero_flag =%u\n",p_s->delta_pic_order_always_zero_flag);
        status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);

        if (status!=0)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid offset_for_non_ref_pic\n");
            return 1;
        }

        p_s->offset_for_non_ref_pic = (t_sint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: offset_for_non_ref_pic =%li\n",p_s->offset_for_non_ref_pic);
        status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
        if (status!=0)
            return 1;
        p_s->offset_for_top_to_bottom_field = (t_sint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: offset_for_top_to_bottom_field =%li\n",p_s->offset_for_top_to_bottom_field);
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

        if ((status!=0) || (svalue > 255))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid num_ref_frames_in_pic_order_cnt_cycle\n");
            return 1;
        }

        p_s->num_ref_frames_in_pic_order_cnt_cycle = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_ref_frames_in_pic_order_cnt_cycle =%d\n",p_s->num_ref_frames_in_pic_order_cnt_cycle);
        for (index=0; index<p_s->num_ref_frames_in_pic_order_cnt_cycle; index++)
        {
            status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
            if (status!=0)
                return 1;
            p_s->offset_for_ref_frame[index] = (t_sint32)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: offset_for_ref_frame[%d] =%li\n",index,p_s->offset_for_ref_frame[index]);
        }
    }

    /* Number of reference frames */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

    if ((status!=0) || (svalue > 16))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid num_ref_frames\n");
        return 1;
    }

    p_s->num_ref_frames = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_ref_frames =%u\n",p_s->num_ref_frames);

  /* added by MC: we have some streams where num_ref_frames = 0, this is clearly an error
   in the stream (makes no sense), so let's conceal it to a reasonable value */
   if(p_s->num_ref_frames==0)
    p_s->num_ref_frames=16;


    /* gaps_in_fame_num_value_allowed flag */
    status = ShowBits(p_b,1,&value);
    if (status!=0)
        return 1;
    p_s->gaps_in_frame_num_value_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: gaps_in_frame_num_value_allowed_flag =%u\n",p_s->gaps_in_frame_num_value_flag);

    /* Picture size */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
    if (status!=0)
        return 1;
    p_s->pic_width_in_mbs_minus1 = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_width_mbs_minus1 =%u\n",p_s->pic_width_in_mbs_minus1);
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
    if (status!=0)
        return 1;
    p_s->pic_height_in_map_units_minus1 = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_height_in_map_units_minus1 =%u\n",p_s->pic_height_in_map_units_minus1);

    /* Frame mbs only */
    status = ShowBits(p_b,1,&value);
    if (status!=0)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: frame_mbs_only_flag\n");
        return 1;
    }

    p_s->frame_mbs_only_flag = (t_uint16)value;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_mbs_only_flag =%u\n",p_s->frame_mbs_only_flag);
    (void)FlushBits(p_b,1);

    p_s->mb_adaptive_frame_field_flag = 0;

    if (!p_s->frame_mbs_only_flag)
    {
        status = ShowBits(p_b,1,&value);
        (void)FlushBits(p_b,1);
        if ( status != 0 )
        {
          OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Error parsing frame_mbs_only_flag\n");
          return 1;
        }
        if ( value == 1 )
        {
          OstTraceFiltInst0( TRACE_WARNING , "H264DEC: BTST: MBAFF not supported\n");
          return 1;
        }

	      p_s->mb_adaptive_frame_field_flag = value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: mb_adaptive_frame_field_flag =%u\n",(unsigned int)value);
    }

    CheckLevelVsFrameSize(p_s);  /* VI 27700 */

    status = ShowBits(p_b,2,&value);
    if (status!=0)
        return 1;

    (void)FlushBits(p_b,2);

    p_s->direct_8x8_inf_flag = (t_uint16)((value>>1)&0x1);
    p_s->frame_cropping_flag = (t_uint16)(value&0x1);

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: direct_8x8_inference_flag =%u\n",p_s->direct_8x8_inf_flag);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_cropping_flag =%u\n",p_s->frame_cropping_flag);

    if (p_s->frame_cropping_flag!=0)
    {
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
        if ((status!=0) || (svalue > ((p_s->pic_width_in_mbs_minus1+1)<<4)-1))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid frame_crop_left_offset\n");
            return 1;
        }
        p_s->frame_crop_left = (t_uint16)svalue;

        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
        if ((status!=0) || (svalue > ((p_s->pic_width_in_mbs_minus1+1)<<4)-1))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid frame_crop_right_offset\n");
            return 1;
        }
        p_s->frame_crop_right = (t_uint16)svalue;

        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
        if ((status!=0) || (svalue > ((p_s->pic_height_in_map_units_minus1+1)<<4)-1))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid frame_crop_top_offset\n");
            return 1;
        }
        p_s->frame_crop_top = (t_uint16)svalue;

        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
        if ((status!=0) || (svalue > ((p_s->pic_height_in_map_units_minus1+1)<<4)-1))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid frame_crop_bottom_offset\n");
            return 1;
        }
        p_s->frame_crop_bottom = (t_uint16)svalue;

        if (p_s->frame_crop_left > ((p_s->pic_width_in_mbs_minus1+1)<<4) - p_s->frame_crop_right - 1)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid cropping parameters\n");
            return 1;
        }

        if (p_s->frame_crop_top > ((p_s->pic_height_in_map_units_minus1+1)<<4) - p_s->frame_crop_bottom - 1)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in sequence parameter set: invalid cropping parameters\n");
            return 1;
        }

        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_crop_left_offset =%u\n",p_s->frame_crop_left);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_crop_right_offset =%u\n",p_s->frame_crop_right);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_crop_top_offset =%u\n",p_s->frame_crop_top);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_crop_bottom_offset =%u\n",p_s->frame_crop_bottom);
    }
    else
    {
        p_s->frame_crop_left = 0;
        p_s->frame_crop_right = 0;
        p_s->frame_crop_top = 0;
        p_s->frame_crop_bottom = 0;
    }

    /* VUI parameters */
    status = ShowBits(p_b,1,&value);
    if (status!=0)
        return 1;
    p_s->vui_pars_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: vui_parameters_present_flag =%u\n",p_s->vui_pars_flag);

    if (p_s->vui_pars_flag)
    {
        status = GetVUI(p_b, &(p_s->vui));
    }
    else
    {
      p_s->vui.timing_info_present_flag = 0;
      p_s->vui.fixed_frame_rate_flag    = 0;
      p_s->vui.sar_width = 1;
      p_s->vui.sar_height = 1;
    }
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
    if(CheckLevelVsDPBSize(p_s)){
		OstTraceFiltInst0( TRACE_DEBUG ,"DPB requirements not met.");
        return 2;
    }

    if(CheckLevelVsResolution(p_s)){
		OstTraceFiltInst0( TRACE_DEBUG ,"Resolution not supported.");
        return 1;
    }
//-Change End for CR336095,ER334368,ER336290 level 5.1. support
    return 0;
}

/**
 * \brief Reads a Subset Sequence Parameters Set NALU.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_nsubsetsp Pointer to variable holding the number of available subset sequence parameter sets
 * \param p_s Array of subset sequence paramet set structures
 * \return 0 if successful, 1 otherwise
 * \author Victor Macela
 * \version 1.0
 *
 * The function reads a Subset Sequence Parameters Set NALU.
 */

t_uint16 METH(GetSubsetSequenceParSet)(t_bit_buffer *p_b, t_sint16 *p_nsubsetsp, t_subset_seq_par *p_subset_sp, t_uint16 * is_mvc_stream)
{
    t_uint32 value;
    t_sint32 svalue;
    t_uint16 status;
    t_uint32 tmp_profile_idc;
    t_uint16 tmp_level_idc;
    t_uint16 additional_extension2_flag;
    t_uint16 i,j,k;
    t_sint16 nsp = 0;
    t_uint16 dummy;

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: ---- SUBSET SEQUENCE PARAMETER SET ----\n");

    /***** MC: pre-read profile IDC before entering GetSPS in order to avoid parsing SVC streams */
    status = ShowBits(p_b,8,&tmp_profile_idc);
    if(status!=0)
      return 1;

    if((tmp_profile_idc == 118) ||(tmp_profile_idc == 128))
      *is_mvc_stream = 1;
    else
      return 0; /* skip/ignore in case of SVC */
    /***** MC: pre-read profile IDC before entering GetSPS in order to avoid parsing SVC streams */

	//Modified the statement below to resolve Coerity warning
    status = GetSequenceParSet(p_b, &nsp, &(dec_static.tmp_sps2), &(dec_static.tmp_sps2), &dummy);

    if (status != 0)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in the sequence parameter set part of the subset sequence parameter\n");
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
        if(status==2) {
			OstTraceFiltInst0( TRACE_DEBUG ,"DPB requirements not met.");
            return 2;
        }
//-Change end for CR336095,ER334368,ER336290 level 5.1. support
        return 1;
    }

    /* Check if this subset sequence parameter set is an update */
    i = 0;
    while ( (i < *p_nsubsetsp) && (p_subset_sp->sp.seq_par_set_id != dec_static.tmp_sps2.seq_par_set_id) )
    {
        i++;
        p_subset_sp++;
    }
    if (i == *p_nsubsetsp)
    {
        (*p_nsubsetsp)++;
    }

    p_subset_sp->sp = dec_static.tmp_sps2;

    if ((dec_static.tmp_sps2.profile_idc == 118) || (dec_static.tmp_sps2.profile_idc == 128))
    {
        /* bit_equal_to_one */
        status = ShowBits(p_b,1,&value);

        if (status!=0)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid bit_equal_to_one\n");
            return 1;
        }

        p_subset_sp->bit_equal_to_one= (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: bit_equal_to_one =%u\n",p_subset_sp->bit_equal_to_one);

        if (p_subset_sp->bit_equal_to_one == 0)
        {
    	    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Warning! bit_equal_to_one is not equal to 1!\n");
        }

        (void)FlushBits(p_b,1);

        /* num_views_minus1 */
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

        if ((status!=0) || (svalue > (MAX_NUM_VIEWS - 1)))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid num_views_minus1\n");
            return 1;
        }

        p_subset_sp->num_views_minus1 = (t_uint16)svalue;

        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_views_minus1 =%u\n",p_subset_sp->num_views_minus1);
        for (i = 0; i <= p_subset_sp->num_views_minus1; i++)
        {
            /* view_id[i] */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > 1023))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid view_id[%u]\n",i);
                return 1;
            }

            p_subset_sp->view_id[i] = (t_uint16)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: view_id[%u] =%u\n",i,p_subset_sp->view_id[i]);
        }

        p_subset_sp->num_anchor_refs_l0[0] = 0;
        p_subset_sp->num_anchor_refs_l1[0] = 0;

        for (i = 1; i <= p_subset_sp->num_views_minus1; i++)
        {
            /* num_anchor_refs_l0[i] */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > MIN(p_subset_sp->num_views_minus1,15)))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid num_anchor_refs_l0[%u]\n",i);
                return 1;
            }

            p_subset_sp->num_anchor_refs_l0[i] = (t_uint16)svalue;

            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: num_anchor_refs_l0[%u] =%u\n",i,p_subset_sp->num_anchor_refs_l0[i]);
            for (j = 0; j < p_subset_sp->num_anchor_refs_l0[i]; j++)
            {
                /* anchor_ref_l0[i][j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if ((status!=0) || (svalue > 1023))
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid anchor_ref_l0[%u][%u]\n",i,j);
                    return 1;
                }

                p_subset_sp->anchor_ref_l0[i][j] = (t_uint16)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: anchor_ref_l0[%u][%u] =%u\n",i,j,p_subset_sp->anchor_ref_l0[i][j]);
            }

            /* num_anchor_refs_l1[i] */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > MIN(p_subset_sp->num_views_minus1,15)))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid num_anchor_refs_l1[%u]\n",i);
                return 1;
            }

            p_subset_sp->num_anchor_refs_l1[i] = (t_uint16)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: num_anchor_refs_l1[%u] =%u\n",i,p_subset_sp->num_anchor_refs_l1[i]);

            for (j = 0; j < p_subset_sp->num_anchor_refs_l1[i]; j++)
            {
                /* anchor_ref_l1[i][j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if ((status!=0) || (svalue > 1023))
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid anchor_ref_l1[%u][%u]\n",i,j);
                    return 1;
                }

                p_subset_sp->anchor_ref_l1[i][j] = (t_uint16)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: anchor_ref_l1[%u][%u] =%u\n",i,j,p_subset_sp->anchor_ref_l1[i][j]);
            }
        }

        p_subset_sp->num_non_anchor_refs_l0[0] = 0;
        p_subset_sp->num_non_anchor_refs_l1[0] = 0;

        for (i = 1; i <= p_subset_sp->num_views_minus1; i++)
        {
            /* num_non_anchor_refs_l0[i] */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > MIN(p_subset_sp->num_views_minus1,15)))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid num_non_anchor_refs_l0[%u]\n",i);
                return 1;
            }

            p_subset_sp->num_non_anchor_refs_l0[i] = (t_uint16)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: num_non_anchor_refs_l0[%u] =%u\n",i,p_subset_sp->num_non_anchor_refs_l0[i]);

            for (j = 0; j < p_subset_sp->num_non_anchor_refs_l0[i]; j++)
            {
                /* non_anchor_ref_l0[i][j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if ((status!=0) || (svalue > 1023))
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid non_anchor_ref_l0[%u][%u]\n",i,j);
                    return 1;
                }

                p_subset_sp->non_anchor_ref_l0[i][j] = (t_uint16)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: non_anchor_ref_l0[%u][%u] =%u\n",i,j,p_subset_sp->non_anchor_ref_l0[i][j]);
            }

            /* num_non_anchor_refs_l1[i] */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > MIN(p_subset_sp->num_views_minus1,15)))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid num_non_anchor_refs_l1[%u]\n",i);
                return 1;
            }

            p_subset_sp->num_non_anchor_refs_l1[i] = (t_uint16)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: num_non_anchor_refs_l1[%u] =%u\n",i,p_subset_sp->num_non_anchor_refs_l1[i]);

            for (j = 0; j < p_subset_sp->num_non_anchor_refs_l1[i]; j++)
            {
                /* non_anchor_ref_l1[i][j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if ((status!=0) || (svalue > 1023))
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid non_anchor_ref_l1[%u][%u]\n",i,j);
                    return 1;
                }

                p_subset_sp->non_anchor_ref_l1[i][j] = (t_uint16)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: non_anchor_ref_l1[%u][%u] =%u\n",i,j,p_subset_sp->non_anchor_ref_l1[i][j]);
            }
        }

        /* num_level_values_signalled_minus1 */
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

        if ((status!=0) || (svalue > (MAX_LEVELS_SIGNALLED - 1)))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid num_level_values_signalled_minus1\n");
            return 1;
        }

        p_subset_sp->num_level_values_signalled_minus1 = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_level_values_signalled_minus1 =%u\n",p_subset_sp->num_level_values_signalled_minus1);

        for (i = 0; i <= p_subset_sp->num_level_values_signalled_minus1; i++)
        {
            /* level_idc[i] */
            status = ShowBits(p_b,8,&value);

            if ((status!=0) || CheckLevel(value, p_subset_sp->sp.profile_idc))
            {
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid level_idc[%u] = %u\n",i,(t_uint16)value);
                return 1;
            }

            p_subset_sp->level_idc[i]= (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: level_idc[%u] =%u\n",i,p_subset_sp->level_idc[i]);
            (void)FlushBits(p_b,8);

            tmp_level_idc = p_subset_sp->sp.level_idc;
            p_subset_sp->sp.level_idc =  p_subset_sp->level_idc[i];
            CheckLevelVsFrameSize(&p_subset_sp->sp);
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
            if(CheckLevelVsDPBSize(&p_subset_sp->sp)){
				OstTraceFiltInst0( TRACE_DEBUG ,"DPB requirements not met.");
                return 2;
            }
//-Change end for CR336095,ER334368,ER336290 level 5.1. support
            p_subset_sp->level_idc[i] = p_subset_sp->sp.level_idc;
            p_subset_sp->sp.level_idc = tmp_level_idc;

            /* num_applicable_ops_minus1[i] */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > (MAX_OPERATION_POINTS - 1)))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid num_applicable_ops_minus1[%u]\n",i);
                return 1;
            }

            p_subset_sp->num_applicable_ops_minus1[i] = (t_uint16)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: num_applicable_ops_minus1[%u] =%u\n",i,p_subset_sp->num_applicable_ops_minus1[i]);

            for (j = 0; j <= p_subset_sp->num_applicable_ops_minus1[i]; j++)
            {
                /* applicable_op_temporal_id[i][j] */
                status = ShowBits(p_b,3,&value);

                if (status!=0)
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid applicable_op_temporal_id[%u][%u]\n", i,j);
                    return 1;
                }

                p_subset_sp->applicable_op_temporal_id[i][j] = (t_uint16)value;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: applicable_op_temporal_id[%u][%u] =%u\n",i,j,p_subset_sp->applicable_op_temporal_id[i][j]);
                (void)FlushBits(p_b,3);

                /* applicable_op_num_target_views_minus1[i][j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if ((status!=0) || (svalue > (MAX_NUM_VIEWS - 1)))
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid applicable_op_num_target_views_minus1[%u][%u]\n",i,j);
                    return 1;
                }

                p_subset_sp->applicable_op_num_target_views_minus1[i][j] = (t_uint16)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: applicable_op_num_target_views_minus1[%u][%u] =%u\n",i,j,p_subset_sp->applicable_op_num_target_views_minus1[i][j]);

                for (k = 0; k <= p_subset_sp->applicable_op_num_target_views_minus1[i][j]; k++)
                {
                    /* applicable_op_target_view_id[i][j][k] */
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                    if ((status!=0) || (svalue > 1023))
                    {
                        OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid applicable_op_target_view_id[%u][%u][%u]\n",i,j,k);
                        return 1;
                    }

                    p_subset_sp->applicable_op_target_view_id[i][j][k] = (t_uint16)svalue;
                    OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: applicable_op_target_view_id[%u][%u][%u]",i,j,k);
                    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST:  =%u\n",p_subset_sp->applicable_op_target_view_id[i][j][k]);
                }

                /* applicable_op_num_views_minus1[i][j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if ((status!=0) || (svalue > (MAX_NUM_VIEWS - 1)))
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid applicable_op_num_views_minus1[%u][%u]\n",i,j);
                    return 1;
                }

                p_subset_sp->applicable_op_num_views_minus1[i][j] = (t_uint16)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: applicable_op_num_views_minus1[%u][%u] =%u\n",i,j,p_subset_sp->applicable_op_num_views_minus1[i][j]);
            }
        }
    }

    /* mvc_vui_parameters_present_flag */
    status = ShowBits(p_b,1,&value);

    if (status!=0)
    {
       OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid mvc_vui_parameters_present_flag\n");
       return 1;
    }

    p_subset_sp->mvc_vui_parameters_present_flag = (t_uint16)value;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: mvc_vui_parameters_present_flag =%u\n",p_subset_sp->mvc_vui_parameters_present_flag);
    (void)FlushBits(p_b,1);

    if (p_subset_sp->mvc_vui_parameters_present_flag)
    {
        status = GetMVCVUI(p_b, &p_subset_sp->mvc_vui_parameters);

        if (status != 0)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in the MVC VUI parameters extension part of the subset sequence parameter\n");
            return 1;
        }
    }

    /* additional_extension2_flag */
    status = ShowBits(p_b,1,&value);

    if (status!=0)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid additional_extension2_flag\n");
        return 1;
    }

    additional_extension2_flag = (t_uint16)value;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: additional_extension2_flag =%u\n",additional_extension2_flag);
    (void)FlushBits(p_b,1);

    /* Quoted from subclause 7.4.2.1.3 Subset sequence parameter set RBSP semantics:
       additional_extension2_flag shall be equal to 0 in bitstreams conforming
       to this Recommendation|International Standard.
       The value of 1 for additional_extension2_flag is reserved for future use by ITU-T|ISO/IEC.
       Decoders shall ignore all data that follow the value 1 for additional_extension2_flag
       in a subset sequence parameter set NAL unit. */
    if (additional_extension2_flag == 1)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Warning! additional_extension2_flag is not equal to 0!\n");
        while (MoreRbspData(p_b))
        {
            /* Quoted from subclause 7.4.2.1.3 Subset sequence parameter set RBSP semantics:
               additional_extension2_data_flag may have any value.
               It shall not affect the conformance to profiles specified in Annex A, G, or H. */
            status = ShowBits(p_b,1,&value);

            if (status!=0)
            {
                OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in subset sequence parameter set: invalid additional_extension2_data_flag\n");
                return 1;
            }

            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: additional_extension2_data_flag =%u\n",(t_uint16)value);
            (void)FlushBits(p_b,1);
        }
    }

    return 0;
}


/**
 * \brief Reads a Picture Parameters Set NALU.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_npp Pointer to variable holding the number of readed picture parameter sets
 * \param p_p Array of picture paramet set structures
 * \return 0 if successful, 1 otherwise
 * \author Denis Zinato
 * \author Filippo Santinello
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>28-06-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>Added error check.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a Picture Parameters Set NALU.
 */

t_uint16 METH(GetPictureParSet)(t_bit_buffer *p_b, t_sint16 *p_npp, t_pic_par *p_p, t_sint16 *p_nsp, t_seq_par *p_s)
{
    t_uint32 value;
    t_sint32 svalue;
    t_uint16 status, bits;
    t_sint16 i;

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: ---- PICTURE PARAMETER SET ----\n");

    /* pic parameter set id */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
    if ((status!=0) || (svalue > 255))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid picture_parameter_set_id\n");
        return 1;
    }

    /* Check if this picture parameter set is an update */
    i = 0;
    while ( (i < *p_npp) && (p_p->pic_par_set_id != (t_uint16)svalue) )
    {
        i++;
        p_p++;
    }
    if (i == *p_npp)
    {
        (*p_npp)++;
    }

    p_p->pic_par_set_id = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_parameter_set_id =%u\n",p_p->pic_par_set_id);

    /* seq parameter set id */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
    if ((status!=0) || (svalue > 31))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid sequence_parameter_set_id\n");
        return 1;
    }

    p_p->seq_par_set_id = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: seq_parameter_set_id =%u\n",p_p->seq_par_set_id);

    /* entropy_mode_coding_flag and pic_order_present_flag */
    status = ShowBits(p_b,2,&value);
    if (status!=0)
        return 1;
    p_p->entropy_mode_flag = (t_uint16)(value>>1);
    p_p->pic_order_present_flag = (t_uint16)(value&0x1);
    (void)FlushBits(p_b,2);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: entropy_mode_coding_flag =%u\n",p_p->entropy_mode_flag);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_order_present_flag =%u\n",p_p->pic_order_present_flag);

    /* num slice groups_minus1 */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
    if ((status!=0) || (svalue > 7))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: num_slice_groups_minus1 should be in the range 0-7 for baseline profile\n");
        return 1;
    }

    p_p->num_slice_groups_minus1 = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_slices_group_minus1 =%u\n",p_p->num_slice_groups_minus1);

    if (p_p->num_slice_groups_minus1!=0)
    {
        /* slice_group_map_type */
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
        if ((status!=0) || (svalue > 6))
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid slice_group_map_type\n");
            return 1;
        }

        p_p->slice_group_map_type = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: slice_group_map_type =%u\n",p_p->slice_group_map_type);

        switch (p_p->slice_group_map_type)
        {
            case 0:
                for (i=0; i<=(t_sint16)p_p->num_slice_groups_minus1; i++)
                {
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                    if (status!=0)
                        return 1;
                    p_p->run_length_minus1[i] = (t_uint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: run_length_minus1[%d] =%u\n",i,p_p->run_length_minus1[i]);
                }
                break;

            case 2:
                if (p_p->num_slice_groups_minus1 != 0)
                {
                    for (i=0; i<(t_sint16)p_p->num_slice_groups_minus1; i++)
                    {
                        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                        if (status!=0)
                            return 1;
                        p_p->top_left[i] = (t_uint16)svalue;
                        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                        if (status!=0)
                            return 1;
                        p_p->bottom_right[i] = (t_uint16)svalue;
                        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: top_left[%d] =%u\n",i,p_p->top_left[i]);
                        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: bottom_right[%d] =%u\n",i,p_p->bottom_right[i]);
                    }
                }
                break;

            case 3:
            case 4:
            case 5:
                /* slice_group_change_direction_flag */
                status = ShowBits(p_b,1,&value);
                if (status!=0)
                    return 1;
                p_p->slice_group_change_dir_flag = (t_uint16)value;
                (void)FlushBits(p_b,1);
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: slice_group_change_direction_flag =%u\n",p_p->slice_group_change_dir_flag);

                /* slice_group_change_rate_minus1 */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                if (status!=0)
                    return 1;
                p_p->slice_group_change_rate_minus1 = (t_uint16)svalue;
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: slice_group_change_rate_minus1 =%u\n",p_p->slice_group_change_rate_minus1);
                break;

            case 6:
                /* Determine ceil(log2(num_slice_groups_minus1 + 1)) */
                bits = 15;
                value = p_p->num_slice_groups_minus1 + 1;
                while (((value>>bits)&0x1)==0)
                    bits--;
                if ((value&((1<<bits)-1))!=0)
                    bits+=1;

                /* pic_size_in_map_units_minus1 */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                if (status!=0)
                    return 1;
                p_p->pic_size_in_map_units_minus1 = (t_uint16)svalue;
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_size_in_map_units_minus1 =%u\n",p_p->pic_size_in_map_units_minus1);

                for (i=0; i<=(t_sint16)p_p->pic_size_in_map_units_minus1; i++)
                {
                    status = ShowBits(p_b,bits,&value);
                    if ((status!=0) || (value > p_p->num_slice_groups_minus1))
                    {
                        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid slice_group_id\n");
                        return 1;
                    }

                    p_p->slice_group_id[i] = (t_uint16)value;
                    (void)FlushBits(p_b,bits);
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: slice_group_id[%d] =%u\n",i,p_p->slice_group_id[i]);
                }
                break;

            default:
            /* Case of slice_group_map_type == 1: do nothing */
            break;
        } /* switch (p_p->slice_group_map_type) */
    }

    /* num ref idx */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
    if ((status!=0) || (svalue > 31))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid num_ref_idx_l0_active_minus1\n");
        return 1;
    }

    p_p->num_ref_idx_l0_active_minus1 = (t_uint16)svalue;

    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

    if ((status!=0) || (svalue > 31))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid num_ref_idx_l1_active_minus1\n");
        return 1;
    }

    p_p->num_ref_idx_l1_active_minus1 = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_ref_idx_l0_active_minus1 =%d\n",p_p->num_ref_idx_l0_active_minus1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_ref_idx_l1_active_minus1 =%d\n",p_p->num_ref_idx_l1_active_minus1);

    /* weighted_pred_flag and weighted_bipred_idc */
    status = ShowBits(p_b,3,&value);
    if (status!=0)
        return 1;
    p_p->weighted_pred_flag = (t_uint16)(value>>2);
    p_p->weighted_bipred_idc = (t_uint16)(value&0x3);
    (void)FlushBits(p_b,3);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: weighted_pred_flag =%u\n",p_p->weighted_pred_flag);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: weighted_bipred_idc =%u\n",p_p->weighted_bipred_idc);

    /* pic_init_qp, pic_init_qs and chroma_qp_index_offset */
    status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
    if ((status!=0) || (svalue < -26) || (svalue > 25))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid pic_init_qp_minus26\n");
        return 1;
    }

    p_p->pic_init_qp_minus26 = (t_sint16)svalue;

    status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
    if ((status!=0) || (svalue < -26) || (svalue > 25))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid pic_init_qs_minus26\n");
        return 1;
    }

    p_p->pic_init_qs_minus26 = (t_sint16)svalue;

    status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
    if ((status!=0) || (svalue < -12) || (svalue > 12))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in picture parameter set: invalid chroma_qp_index_offset\n");
        return 1;
    }

    p_p->chroma_qp_index_offset = (t_sint16)svalue;

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_init_qp_minus26 =%d\n",p_p->pic_init_qp_minus26);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_init_qs_minus26 =%d\n",p_p->pic_init_qs_minus26);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: chroma_qp_index_offset =%d\n",p_p->chroma_qp_index_offset);

    /* deblocking_filter_control_present, constrained_intra_pred and redundant_pic_cnt_present flags */
    status = ShowBits(p_b,3,&value);
    if (status!=0)
        return 1;
    p_p->debl_flt_control_present_flag = (t_uint16)(value>>2);
    p_p->constr_intra_pred_flag = (t_uint16)((value>>1)&0x1);
    p_p->redundant_pic_cnt_present_flag = (t_uint16)(value&0x1);
    (void)FlushBits(p_b,3);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: deblocking_filter_control_present_flag =%u\n",p_p->debl_flt_control_present_flag);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: constrained_intra_pred_flag =%u\n",p_p->constr_intra_pred_flag);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: redundant_pic_cnt_present_flag =%u\n",p_p->redundant_pic_cnt_present_flag);

    p_p->transform_8x8_mode_flag = 0;
    p_p->pic_scaling_matrix_present_flag = 0;

    if (MoreRbspData(p_b)) {
      status = ShowBits(p_b,2,&value);
      if (status!=0)
        return 1;
      p_p->transform_8x8_mode_flag = (t_uint16)(value>>1);
      p_p->pic_scaling_matrix_present_flag = (t_uint16)(value&0x1);
      (void)FlushBits(p_b,2);

      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: transform_8x8_mode_flag =%u\n",p_p->transform_8x8_mode_flag);
      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_scaling_matrix_present_flag =%u\n",p_p->pic_scaling_matrix_present_flag);

      if (p_p->pic_scaling_matrix_present_flag) {
        for (i =0; i<(6+(2*p_p->transform_8x8_mode_flag)); i++) {
          /* NZ: 6 + ((chroma_format_idc!=3 ? 2:6)*transform_8x8_mode_flag) ==> chroma_format_idc==1 ALWAYS in 420 */
          if (ShowBits(p_b,1,&value))
            return 1;
          p_p->pic_scaling_list_present_flag[i] = (t_uint16)value;
          (void)FlushBits(p_b,1);
          OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: pic_scaling_list_present_flag[%d]=%u\n",i, p_p->pic_scaling_list_present_flag[i]);

        if (p_p->pic_scaling_list_present_flag[i]) {
            if (i<6)
            {
              status = ScalingList(p_b, i  , &p_p->pic_scaling_list, SCALING_LIST_4x4);
              if(p_p->pic_scaling_list.UseDefaultScalingMatrix4x4Flag[i])
                SetDefault_ScalingList(i,&p_p->pic_scaling_list,SCALING_LIST_4x4);
            }
            else
            {
              status = ScalingList(p_b, i-6, &p_p->pic_scaling_list, SCALING_LIST_8x8);
              if(p_p->pic_scaling_list.UseDefaultScalingMatrix8x8Flag[i-6])
                SetDefault_ScalingList(i-6,&p_p->pic_scaling_list,SCALING_LIST_8x8);
            }
            if (status)
              return 1;
          } /* NZ: if (p_p->pic_scaling_list_present_flag[i]) */
          else
          {
            if(i<6)
              SetDefault_ScalingList_setB(i,&p_p->pic_scaling_list,SCALING_LIST_4x4,p_p->seq_par_set_id,p_s,p_nsp);
            else
              SetDefault_ScalingList_setB(i-6,&p_p->pic_scaling_list,SCALING_LIST_8x8,p_p->seq_par_set_id,p_s,p_nsp);
          }
        } /* for (...) */

        /* it is not used but let's set it properly to have clean parameters */
        if(p_p->transform_8x8_mode_flag==0)
        {
          SetDefault_ScalingList_setB(0,&p_p->pic_scaling_list,SCALING_LIST_8x8,p_p->seq_par_set_id,p_s,p_nsp);
          SetDefault_ScalingList_setB(1,&p_p->pic_scaling_list,SCALING_LIST_8x8,p_p->seq_par_set_id,p_s,p_nsp);
        }
      } /* if (p_p->pic_scaling_matrix_present_flag)*/

      /* + change for ER 421364 */
      OstTraceFiltInst0( TRACE_FLOW , "H264DEC: BTST: Abt to retrieve value of second_chroma_qp_index_offset");
      if (GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue))
      {
		  p_p->second_chroma_qp_index_offset = p_p->chroma_qp_index_offset;
		  OstTraceFiltInst1( TRACE_FLOW , "H264DEC: BTST: second_chroma_qp_index_offset=%d (EQUAL TO chroma_qp_index_offset) \n", p_p->second_chroma_qp_index_offset);
	  }
	  else
	  {
          p_p->second_chroma_qp_index_offset = (t_sint16)svalue;
          OstTraceFiltInst1( TRACE_FLOW , "H264DEC: BTST: second_chroma_qp_index_offset=%d\n", p_p->second_chroma_qp_index_offset);
	  }
      /* - change for ER 421364 */
    } /* more_rbsp_data */
    else
    {
      p_p->second_chroma_qp_index_offset      = p_p->chroma_qp_index_offset;
    }

    return 0;
}


const t_uint8 SCAL_ZZ_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

const t_uint8 SCAL_ZZ_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
   12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
   35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
   58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};


/*
  default scaling lists
*/
const t_uint32 host_quant_intra_default[16] = {
 6,13,20,28,
13,20,28,32,
20,28,32,37,
28,32,37,42
};

const t_uint32 host_quant_inter_default[16] = {
10,14,20,24,
14,20,24,27,
20,24,27,30,
24,27,30,34
};

const t_uint32 host_quant8_intra_default[64] = {
 6,10,13,16,18,23,25,27,
10,11,16,18,23,25,27,29,
13,16,18,23,25,27,29,31,
16,18,23,25,27,29,31,33,
18,23,25,27,29,31,33,36,
23,25,27,29,31,33,36,38,
25,27,29,31,33,36,38,40,
27,29,31,33,36,38,40,42
};

const t_uint32 host_quant8_inter_default[64] = {
 9,13,15,17,19,21,22,24,
13,13,17,19,21,22,24,25,
15,17,19,21,22,24,25,27,
17,19,21,22,24,25,27,28,
19,21,22,24,25,27,28,30,
21,22,24,25,27,28,30,32,
22,24,25,27,28,30,32,33,
24,25,27,28,30,32,33,35
};



/**
 * \brief Parse Scaling List syntax element
 * \param p_b Pointer to bit buffer structure
 * \return 1 if errors, 0 otherwise.
 * \author Nicola ZANDONA'
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>05-03-2008&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function parse the scaling list syntax element
 */

t_uint16 ScalingList(t_bit_buffer *p_b, t_uint16 slist_index, tps_ScalingList p_scaling_list,
    te_ScalingListType sl_kind) {
  t_uint16 lastScale = 8;
  t_uint16 nextScale = 8;
  t_uint16 j,scanj;
  t_sint32 deltascale;
  t_uint16 sizeOfScalingList;
  t_sint16 *scalingList;
  t_uint16 *matrixFlag;

  if (sl_kind==SCALING_LIST_4x4)
  {
    sizeOfScalingList = 16;
    scalingList = p_scaling_list->ScalingList4x4[slist_index];
    matrixFlag =  p_scaling_list->UseDefaultScalingMatrix4x4Flag;
  }
  else
  {
    sizeOfScalingList = 64;
    scalingList = p_scaling_list->ScalingList8x8[slist_index];
    matrixFlag =  p_scaling_list->UseDefaultScalingMatrix8x8Flag;
  }

  for ( j = 0; j < sizeOfScalingList; j++ )
  {
    scanj = (sizeOfScalingList==16) ? SCAL_ZZ_SCAN[j]:SCAL_ZZ_SCAN8[j];

    if ( nextScale != 0 )
    {
      if (GetExpGolomb(p_b,EXPGOLOMB_SE,&deltascale))
        return 1;
      nextScale = ( lastScale + deltascale + 256 ) % 256;
      matrixFlag[slist_index] = ( scanj==0 && nextScale==0 );
    }
    scalingList[scanj] = ( nextScale==0 ) ? lastScale : nextScale;
    lastScale = scalingList[scanj];
  }
  return 0;
}

/*
  function to assign default scaling lists (see tables 7-3/7-4 in the std)
  */
t_uint16 SetDefault_ScalingList(t_uint16 slist_index, tps_ScalingList p_scaling_list, te_ScalingListType sl_kind) {
  t_uint16 j;
  t_sint16 *scalingList;

  if (sl_kind==SCALING_LIST_4x4)
  {
    scalingList = p_scaling_list->ScalingList4x4[slist_index];
    switch(slist_index)
    {
      case 0: /* default scaling list */
      case 1:
      case 2:
      default:
        for(j=0;j<16;j++)
          {
            scalingList[j] = host_quant_intra_default[j];
          }
        break;
      case 3: /* default scaling list */
      case 4:
      case 5:
       for(j=0;j<16;j++)
          {
            scalingList[j] = host_quant_inter_default[j];
          }
        break;
    }
  }
  else
  {
    scalingList = p_scaling_list->ScalingList8x8[slist_index];
    switch(slist_index)
    {
      case 0:
      default:
       for(j=0;j<64;j++)
          {
            scalingList[j] = host_quant8_intra_default[j];
          }
        break;
      case 1:
       for(j=0;j<64;j++)
          {
            scalingList[j] = host_quant8_inter_default[j];
          }
        break;
    }
  }

  return 0;
}


/*
  Table 7-2 in standard, scaling list fallback rule set A
  */
t_uint16 SetDefault_ScalingList_setA(t_uint16 slist_index, tps_ScalingList p_scaling_list, te_ScalingListType sl_kind) {
  t_uint16 j;
  t_sint16 *scalingList;
  t_sint16 *prev_scalingList;

  if (sl_kind==SCALING_LIST_4x4)
  {
    scalingList = p_scaling_list->ScalingList4x4[slist_index];
    switch(slist_index)
    {
      case 0: /* default scaling list */
      default:
        for(j=0;j<16;j++)
          {
            scalingList[j] = host_quant_intra_default[j];
          }
        break;
      case 3: /* default scaling list */
       for(j=0;j<16;j++)
          {
            scalingList[j] = host_quant_inter_default[j];
          }
        break;
      case 1: /* scaling list for i-1 */
      case 2:
      case 4:
      case 5:
        prev_scalingList = p_scaling_list->ScalingList4x4[slist_index-1];
        for(j=0;j<16;j++)
          {
            scalingList[j] = prev_scalingList[j];
          }
        break;
    }
  }
  else
  {
    scalingList = p_scaling_list->ScalingList8x8[slist_index];
    switch(slist_index)
    {
      case 0:
      default:
       for(j=0;j<64;j++)
          {
            scalingList[j] = host_quant8_intra_default[j];
          }
        break;
      case 1:
       for(j=0;j<64;j++)
          {
            scalingList[j] = host_quant8_inter_default[j];
          }
        break;
    }
  }

  return 0;
}


/*
  Table 7-2 in standard, scaling list fallback rule set B
  */
t_uint16 SetDefault_ScalingList_setB(t_uint16 slist_index, tps_ScalingList p_scaling_list,
                                       te_ScalingListType sl_kind, t_uint16 sps_id,t_seq_par * p_stab,t_sint16* p_nsp) {
  t_uint16 j,i, index_sps=0;
  t_sint16 *scalingList;
  t_sint16 *prev_scalingList;
  t_seq_par *p_s = p_stab;

   i = 0;
   while ((p_s->seq_par_set_id != sps_id)&&(index_sps++<*p_nsp))
   {
      i++;
      p_s++;
   }

  if (sl_kind==SCALING_LIST_4x4)
  {
    scalingList = p_scaling_list->ScalingList4x4[slist_index];
    switch(slist_index)
    {
      case 0: /* default scaling list is sequence-level scaling list */
      default:
        if(p_s->seq_scaling_matrix_present_flag)
        {
          for(j=0;j<16;j++)
              scalingList[j] = p_s->seq_scaling_list.ScalingList4x4[slist_index][j];
        }
        else
        {
          for(j=0;j<16;j++)
              scalingList[j] = host_quant_intra_default[j];
        }
        break;
      case 3: /* default scaling list */
        if(p_s->seq_scaling_matrix_present_flag)
        {
          for(j=0;j<16;j++)
              scalingList[j] = p_s->seq_scaling_list.ScalingList4x4[slist_index][j];
        }
        else
        {
          for(j=0;j<16;j++)
              scalingList[j] = host_quant_inter_default[j];
        }
        break;
      case 1: /* scaling list for i-1 */
      case 2:
      case 4:
      case 5:
        prev_scalingList = p_scaling_list->ScalingList4x4[slist_index-1];
        for(j=0;j<16;j++)
          {
            scalingList[j] = prev_scalingList[j];
          }
        break;
    }
  }
  else
  {
    scalingList = p_scaling_list->ScalingList8x8[slist_index];
    switch(slist_index)
    {
      case 0:
      default:
        if(p_s->seq_scaling_matrix_present_flag)
        {
          for(j=0;j<64;j++)
              scalingList[j] = p_s->seq_scaling_list.ScalingList8x8[slist_index][j];
        }
        else
        {
          for(j=0;j<64;j++)
           scalingList[j] = host_quant8_intra_default[j];
        }
        break;
      case 1:
        if(p_s->seq_scaling_matrix_present_flag)
        {
          for(j=0;j<64;j++)
              scalingList[j] = p_s->seq_scaling_list.ScalingList8x8[slist_index][j];
        }
        else
        {
          for(j=0;j<64;j++)
           scalingList[j] = host_quant8_inter_default[j];
        }
        break;
    }
  }

  return 0;
}




/**
 * \brief Find if there is more data in the slice
 * \param p_b Pointer to bit buffer structure
 * \return 1 if there is more data in the slice, 0 otherwise.
 * \author Denis Zinato
 * \author Nicola ZANDONA'
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>11-11-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>05-03-2008&nbsp;</td><td>NZ: Adapted from MoreData() in hamac_bitstream.c.</td></tr>
 * </table>
 * </dl>
 *
 * The function find if there is more data (used in GetPictureParSet()).
 */

t_uint16 MoreRbspData(t_bit_buffer *p_b)
{
    t_uint32 value;
    t_uint16 stopbyte;
    t_uint8 *p_tmp = (t_uint8 *) p_b->addr;
    t_uint16 nbits;
    t_uint16 flag = 0;

    p_tmp++;

    if (p_b->os == 0)
    {
        nbits = 8;
        p_tmp++;
    }
    else
        nbits = p_b->os;

    if(ShowBits(p_b,nbits,&value))  /* Read stop bit and remaining bits of byte */
        return 0;

    stopbyte = 1 << (nbits-1);

    if ( ((t_uint8 *)p_b->end - p_tmp) > 2 )
    {
        if ((value == stopbyte) && (*(p_tmp) == 0) && (*(p_tmp+1) == 0) && ( (*(p_tmp+2) == 0) || (*(p_tmp+2) == 1) ) )
            return 0;
        else
            return 1;
    }
    else if ( ((t_uint8 *)p_b->end - p_tmp) >= 0 )
    {
        for (;((t_uint8 *)p_b->end - p_tmp) >= 0;)
            flag |= *(p_tmp++);

        if ((value == stopbyte) && (flag == 0) )
            return 0;
        else
            return 1;
    }
    else  /* Last byte */
    {
        if (value == stopbyte)
            return 0;
        else
            return 1;
    }
}



t_uint16 pred_weight_table1(t_bit_buffer *p_b,t_slice_hdr *p_s) {

  t_uint16 status,i,j;
  t_sint32 svalue;
  t_uint32 value;

  /* luma_log2_weight_denom */
  status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
  if ( (status!=0) || ((t_uint16)svalue>7) )
  {
#if VERB_ERR_CONC == 1
    NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid luma_log2_weight_denom\n", p_s->slice_num);
#endif
      return 1;
  }
  p_s->luma_log2_weight_denom = (t_uint16)svalue;
#if VERBOSE_BITSTREAM > 0
      NMF_LOG("luma_log2_weight_denom =%d\n", p_s->luma_log2_weight_denom);
#endif

  /* chroma_log2_weight_denom */
  status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
  if ( (status!=0) || ((t_uint16)svalue>7) )
  {
#if VERB_ERR_CONC == 1
    NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid chroma_log2_weight_denom\n", p_s->slice_num);
#endif
      return 1;
  }
  p_s->chroma_log2_weight_denom = (t_uint16)svalue;
#if VERBOSE_BITSTREAM > 0
      NMF_LOG("chroma_log2_weight_denom =\t%d\n", p_s->chroma_log2_weight_denom);
#endif

  for ( i=0; i<=p_s->num_ref_idx_l0_active_minus1; i++)
  {
    /* luma_weight_l0_flag */
    if ( ShowBits(p_b,1,&value) )
      return 1;
    (void)FlushBits(p_b,1);
#if VERBOSE_BITSTREAM > 0
      NMF_LOG("luma_weight_l0_flag =\t%ld\n", value);
#endif

    if ( value )
    {
      /* luma_weight_l0[i] */
      status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
      if ( (status!=0) || (svalue < -128) || (svalue > 127) )
      {
#if VERB_ERR_CONC == 1
        NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid luma_weight_l0[%d]\n", p_s->slice_num,i);
#endif
        return 1;
      }
      p_s->luma_weight_l0[i] = (t_sint16) svalue;
#if VERBOSE_BITSTREAM > 0
      NMF_LOG("luma_weight_l0[%d] =\t%d\n", i, p_s->luma_weight_l0[i]);
#endif

      /* luma_offset_l0[i] */
      status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
      if ( (status!=0) || (svalue < -128) || (svalue > 127) )
      {
#if VERB_ERR_CONC == 1
        NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid luma_offset_l0[%d]\n", p_s->slice_num,i);
#endif
        return 1;
      }
      p_s->luma_offset_l0[i] = (t_sint16) svalue;
#if VERBOSE_BITSTREAM > 0
      NMF_LOG("luma_offset_l0[%d] =\t%d\n", i, p_s->luma_offset_l0[i]);
#endif

    }
    else /* inferred values in case luma_weight_l0_flag == 0 */
    {
      p_s->luma_weight_l0[i] = 1<<(p_s->luma_log2_weight_denom); /* 2^(p_s->luma_log2_weight_denom)*/
      p_s->luma_offset_l0[i] = 0;
    }

    /* chroma_weight_l0_flag */
    if ( ShowBits(p_b,1,&value) )
      return 1;
    (void)FlushBits(p_b,1);
#if VERBOSE_BITSTREAM > 0
      NMF_LOG("chroma_weight_l0_flag =\t%ld\n", value);
#endif

    if ( value )
    {
      for ( j=0; j<2; j++)
      {
        /* chroma_weight_l0[i][j] */
        status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
        if ( (status!=0) || (svalue < -128) || (svalue > 127) )
        {
#if VERB_ERR_CONC == 1
          NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid chroma_weight_l0[%d][%d] %ld\n", p_s->slice_num,i,j,svalue);
#endif
         return 1;
        }
        p_s->chroma_weight_l0[i][j] = (t_sint16) svalue;
#if VERBOSE_BITSTREAM > 0
        NMF_LOG("chroma_weight_l0[%d][%d] =\t%d\n", i,j, p_s->chroma_weight_l0[i][j]);
#endif

        /* chroma_offset_l0[i][j] */
        status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
        if ( (status!=0) || (svalue < -128) || (svalue > 127) )
        {
#if VERB_ERR_CONC == 1
          NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid chroma_offset_l0[%d][%d] %ld\n", p_s->slice_num,i,j,svalue);
#endif
         return 1;
        }
        p_s->chroma_offset_l0[i][j] = (t_sint16) svalue;
#if VERBOSE_BITSTREAM > 0
        NMF_LOG("chroma_offset_l0[%d][%d] =\t%d\n", i,j, p_s->chroma_offset_l0[i][j]);
#endif
      }
    }
    else
    {
      for ( j=0; j<2; j++)
      {
        p_s->chroma_weight_l0[i][j] = 1<<(p_s->chroma_log2_weight_denom);
        p_s->chroma_offset_l0[i][j] = 0;
      }
    }
  }


  if ((p_s->slice_type==1) || (p_s->slice_type==6)) /* B SLICES */
  {
    for ( i=0; i<=p_s->num_ref_idx_l1_active_minus1; i++)
    {
      /* luma_weight_l1_flag */
      if ( ShowBits(p_b,1,&value) )
        return 1;
      (void)FlushBits(p_b,1);

      if ( value )
      {
        /* luma_weight_l1[i] */
        status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
        if ( (status!=0) || (svalue < -128) || (svalue > 127) )
        {
#if VERB_ERR_CONC == 1
          NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid luma_weight_l1[%d]\n", p_s->slice_num,i);
#endif
          return 1;
        }
        p_s->luma_weight_l1[i] = (t_sint16) svalue;

#if VERBOSE_BITSTREAM > 0
       NMF_LOG("luma_weight_l1[%d] =\t%d\n", i, p_s->luma_weight_l1[i]);
#endif
        /* luma_offset_l1[i] */
        status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
        if ( (status!=0) || (svalue < -128) || (svalue > 127) )
        {
#if VERB_ERR_CONC == 1
          NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid luma_offset_l1[%d]\n", p_s->slice_num,i);
#endif
          return 1;
        }
        p_s->luma_offset_l1[i] = (t_sint16) svalue;
#if VERBOSE_BITSTREAM > 0
       NMF_LOG("luma_weight_l1[%d] =\t%d\n", i, p_s->luma_offset_l1[i]);
#endif
      }
      else /* inferred values in case luma_weight_l1_flag == 0 */
      {
        p_s->luma_weight_l1[i] = 1<<(p_s->luma_log2_weight_denom); /* 2^(p_s->luma_log2_weight_denom)*/
        p_s->luma_offset_l1[i] = 0;
      }

      /* chroma_weight_l1_flag */
      if ( ShowBits(p_b,1,&value) )
        return 1;
      (void)FlushBits(p_b,1);

      if ( value )
      {
        for ( j=0; j<2; j++)
        {
          /* chroma_weight_l1[i][j] */
          status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
          if ( (status!=0) || (svalue < -128) || (svalue > 127) )
          {
#if VERB_ERR_CONC == 1
            NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid chroma_weight_l1[%d][%d]\n", p_s->slice_num,i,j);
#endif
           return 1;
          }
          p_s->chroma_weight_l1[i][j] = (t_sint16) svalue;
#if VERBOSE_BITSTREAM > 0
        NMF_LOG("chroma_weight_l1[%d][%d] =\t%d\n", i,j, p_s->chroma_weight_l1[i][j]);
#endif
          /* chroma_offset_l1[i][j] */
          status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);
          if ( (status!=0) || (svalue < -128) || (svalue > 127) )
          {
#if VERB_ERR_CONC == 1
            NMF_LOG("\nSyntax error in slice %i header (pred_weight_table): invalid chroma_offset_l1[%d][%d]\n", p_s->slice_num,i,j);
#endif
           return 1;
          }
          p_s->chroma_offset_l1[i][j] = (t_sint16) svalue;
#if VERBOSE_BITSTREAM > 0
        NMF_LOG("chroma_offset_l1[%d][%d] =\t%d\n", i,j, p_s->chroma_offset_l1[i][j]);
#endif
        }
      }
      else
      {
        for ( j=0; j<2; j++)
        {
          p_s->chroma_weight_l1[i][j] = 1<<(p_s->chroma_log2_weight_denom);
          p_s->chroma_offset_l1[i][j] = 0;
        }
      }
    }
  }

  return 0;
}



/**
 * \brief Reads the header part of a Slice.
 * \param p_b Pointer to the bit_buffer struct
 * \param buf Pointer to the decoder buffer structure
 * \param p_q Array of Sequence Parameter Sets
 * \param p_p Array of Picture Parameter Sets
 * \param num_pic_par Number of picture parameter sets available
 * \param num_seq_par Number of sequence parameter sets available
 * \param first_slice Flag indicating the presence of a previous parsed slice
 * \param p_old_sl Pointer to the old slice structure
 * \param p_s Pointer to the slice_hdr struct
 * \return 0 if successful, 1 on error, 2 for drop slice, 3 for redundant slice, 4 for stop decoding
 * \author Filippo Santinello
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>02-07-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>Modified interface. Added error detection.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads the header part of a slice performing also syntax check of parsed parameter.
 */

t_uint16 METH(GetSliceHeader)(t_bit_buffer *p_b, t_dec_buff *buf, void *p_q, t_pic_par *p_p, t_uint16 num_pic_par,
                        t_uint16 num_seq_par, t_uint16 num_subset_seq_par,
                        t_uint16 first_slice, t_old_slice *p_old_sl, t_slice_hdr *p_s)
{
    t_uint32 value;
    t_sint32 svalue;
    t_uint16 status, bits, terminate, res;
    t_sint16 index;
    t_sint16 i;
    t_uint16 mmco5_flag = 0;
    t_uint16 num_ref_frames=0;
    t_uint16 num_ref_frames_l1 = 0;
    t_subset_seq_par *p_subset_sp;
    t_seq_par *p_sp;
    t_pic_par *p_pp;


    p_s->error_type = GENERIC;

    p_s->svc_extension_flag = -1;

    p_subset_sp = (t_subset_seq_par *)p_q;
    p_sp = (t_seq_par *)p_q;

    if ((p_s->nut == 14 /*NALU_TYPE_PREFIX*/) || (p_s->nut == 20 /*NALU_TYPE_SLC_EXT*/))
    {
        status = ShowBits(p_b,1,&value);

        if ((status!=0) || (value==1))
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid svc_extension_flag\n", p_s->slice_num);
            return 1;
        }

        p_s->svc_extension_flag = (t_sint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: svc_extension_flag =%u\n",p_s->svc_extension_flag);
        (void)FlushBits(p_b,1);

        status = ShowBits(p_b,1,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid non_idr_flag\n", p_s->slice_num);
            return 1;
        }

        p_s->NaluHeaderMVCExt.non_idr_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Nalu Header MVC Extension: non_idr_flag =%u\n",p_s->NaluHeaderMVCExt.non_idr_flag);
        (void)FlushBits(p_b,1);

        status = ShowBits(p_b,6,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid priority_id\n", p_s->slice_num);
            return 1;
        }

        p_s->NaluHeaderMVCExt.priority_id = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Nalu Header MVC Extension: priority_id =%u\n",p_s->NaluHeaderMVCExt.priority_id);
        (void)FlushBits(p_b,6);

        status = ShowBits(p_b,10,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid view_id\n", p_s->slice_num);
            return 1;
        }

        p_s->NaluHeaderMVCExt.view_id = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Nalu Header MVC Extension: view_id =%u\n",p_s->NaluHeaderMVCExt.view_id);
        (void)FlushBits(p_b,10);

        status = ShowBits(p_b,3,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid temporal_id\n", p_s->slice_num);
            return 1;
        }

        p_s->NaluHeaderMVCExt.temporal_id = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Nalu Header MVC Extension: temporal_id =%u\n",p_s->NaluHeaderMVCExt.temporal_id);
        (void)FlushBits(p_b,3);

        status = ShowBits(p_b,1,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid anchor_pic_flag\n", p_s->slice_num);
            return 1;
        }

        p_s->NaluHeaderMVCExt.anchor_pic_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Nalu Header MVC Extension: anchor_pic_flag =%u\n",p_s->NaluHeaderMVCExt.anchor_pic_flag);
        (void)FlushBits(p_b,1);

        status = ShowBits(p_b,1,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid inter_view_flag\n", p_s->slice_num);
            return 1;
        }

        p_s->NaluHeaderMVCExt.inter_view_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Nalu Header MVC Extension: inter_view_flag =%u\n",p_s->NaluHeaderMVCExt.inter_view_flag);
        (void)FlushBits(p_b,1);

        status = ShowBits(p_b,1,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid reserved_one_bit\n", p_s->slice_num);
            return 1;
        }

        p_s->NaluHeaderMVCExt.reserved_one_bit = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Nalu Header MVC Extension: reserved_one_bit =%u\n",p_s->NaluHeaderMVCExt.reserved_one_bit);

        if (p_s->NaluHeaderMVCExt.reserved_one_bit == 0)
        {
	         OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Warning! Nalu Header MVC Extension: reserved_one_bit is not 1!\n");
        }

        (void)FlushBits(p_b,1);

        if (p_s->nut == 14 /*NALU_TYPE_PREFIX*/)
        {
            p_s->NaluHeaderMVCExt.iPrefixNALU = 1;
            return OK;
        }
        else
        {
            p_s->NaluHeaderMVCExt.iPrefixNALU = 0;

            p_s->nut = p_s->NaluHeaderMVCExt.non_idr_flag==0? 5 : 1;

            p_sp = &p_subset_sp->sp;
        }
    }

    /* first_mb_in_slice */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

    if (status!=0)
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid first macroblock in slice\n", p_s->slice_num);
        p_s->error_type = FIRST_MB_IN_SLICE;
        return 1;
    }

    p_s->first_mb_in_slice = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: first_mb_in_slice =%u\n",p_s->first_mb_in_slice);

    /* slice_type */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

    /* There is maybe a problem with the following test regarding the use of profile_idc.
       Indeed, this parameter is extracted from a sequence parameter set or a subset sequence parameter set
       which may not be the one used by the current slice (pic_parameter_set_id is not yet known and therefore
       neither is seq_parameter_set_id).
       In the context of the MVC extension, this test is kept but it should be further checked.
       Finally, note that the part of the test that uses profile_idc is not present in the Baseline reference code. */
    if ( ( svalue>= 10) || (status!=0) || ( (p_s->svc_extension_flag == -1) && (p_s->nut == 5) && ((svalue%5) != 2) ) ||
        ( (p_sp->profile_idc==66) & ((svalue%5) != 0) & ((svalue%5) != 2) ) || /* <- Baseline */
        ( ((svalue%5) != 0) & ((svalue%5) != 2) & ((svalue%5) != 1) ) ) /* <- Main/High */
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid slice type\n", p_s->slice_num);
        p_s->error_type = SLICE_TYPE;
        return 1;
    }

    p_s->slice_type = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: slice_type =%u (",p_s->slice_type);

    switch(p_s->slice_type)
    {
        case 0:
        case 5:
            OstTraceFiltInst0( TRACE_DEBUG , "P slice)\n");
        break;

        case 2:
        case 7:
            OstTraceFiltInst0( TRACE_DEBUG , "I slice)\n");
        break;

        case 1:
        case 6:
            OstTraceFiltInst0( TRACE_DEBUG , "B slice)\n");
        break;

        default:
        break;
    }

    /* pic_parameter_set_id */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

    if (status!=0)
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid pic_par_set_id\n", p_s->slice_num);
        p_s->error_type = PIC_PAR_SET_ID;
        return 1;
    }

    p_s->pic_par_set_id = (t_uint16)svalue;

    i = 0;
    while ((i < num_pic_par) && (p_p->pic_par_set_id != p_s->pic_par_set_id))
    {
        i++;
        p_p++;
    }

    if (i == num_pic_par)
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid pic_par_set_id\n", p_s->slice_num);
        p_s->error_type = PIC_PAR_SET_ID;
        return 1;
    }

    p_pp = p_p;
    p_s->active_pp = p_pp;

    i = 0;

    if (p_s->svc_extension_flag == -1)
    {
        while ((i < num_seq_par) && (p_sp->seq_par_set_id != p_pp->seq_par_set_id))
        {
            i++;
            p_sp++;
        }

        if (i == num_seq_par)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in active picture parameter set: invalid seq_par_set_id\n");
            return STOP;
        }

        p_s->active_sp = p_sp;
    }
    else
    {
        while ((i < num_subset_seq_par) && (p_subset_sp->sp.seq_par_set_id != p_pp->seq_par_set_id))
        {
            i++;
            p_subset_sp++;
        }

        if (i == num_subset_seq_par)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in active picture parameter set: invalid seq_par_set_id\n");
            return STOP;
        }

        p_sp = &p_subset_sp->sp;
        p_s->active_sp = p_sp;
    }

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_parameter_set_id =%u\n",p_s->pic_par_set_id);

    /* Now that frame size is known, check first_mb_in_slice */
    if ( p_s->first_mb_in_slice >= ( (p_sp->pic_width_in_mbs_minus1+1) *
                                   ( (p_sp->pic_height_in_map_units_minus1+1))*(2-p_sp->frame_mbs_only_flag)) ) /* Modified by D.F. for High Profile */
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid first macroblock in slice!\n", p_s->slice_num);
        p_s->error_type = FIRST_MB_IN_SLICE;
        return 1;
    }

    /* frame_num */
    bits = p_sp->log2_max_frame_num_minus4 + 4;  /* get the max frame number bits */
    status = ShowBits(p_b,bits,&value);
    if (status!=0)
        return 1;

    if ((p_s->nut != 5) && (p_pp->redundant_pic_cnt_present_flag==0) && CheckFrameNum(value, p_s->nri, first_slice, p_old_sl, p_sp, buf))
    {
        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid frame_num %li\n", p_s->slice_num, value);
        p_s->error_type = FRAME_NUM;
#if CONC_FRAME_NUM == 1
        return 1;
#else
        return DROP;
#endif
    }

    p_s->frame_num = (t_uint16)value;
    (void)FlushBits(p_b,bits);

    if ((p_s->nut == 5) && (p_s->frame_num != 0))
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: frame_num of IDR must be 0\n", p_s->slice_num);
#if CONC_FRAME_NUM == 1
        p_s->error_type = FRAME_NUM;
        return 1;
#else
        p_s->frame_num = 0;
#endif
    }

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_num =%u\n", p_s->frame_num);

    /* field_pic_flag and bottom_field_flag are not present in baseline profile */
    if (!p_sp->frame_mbs_only_flag)
    {
        status = ShowBits(p_b,1,&value);
        (void)FlushBits(p_b,1);

        if ( (status!=0) || (value==1)){
          OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Field coding not supported\n");
          return STOP;
        }
    }

    /* idr_pic_id */
    if (p_s->nut == 5)
    {
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

        if ((status!=0) || (svalue > 65535))
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid idr_pic_id\n", p_s->slice_num);
            p_s->error_type = IDR_ID;
            return 1;
        }

        p_s->idr_pic_id = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: idr_pic_id =%u\n",p_s->idr_pic_id);
    }

    /* pic_order_cnt_lsb */
    if (p_sp->pic_order_cnt_type==0)
    {
        bits = p_sp->log2_max_pic_order_cnt_lsb_minus4 + 4;
        status = ShowBits(p_b,bits,&value);
        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid pic_order_cnt_lsb\n", p_s->slice_num);
            return 1;
        }
        (void)FlushBits(p_b,bits);
        p_s->pic_order_cnt_lsb = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_order_cnt_lsb =%u\n",p_s->pic_order_cnt_lsb);

        /* delta_pic_order_cnt_bottom */
        if (p_pp->pic_order_present_flag==1)
        {
            status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);

            if (status!=0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid delta_pic_order_cnt_bottom\n", p_s->slice_num);
                return 1;
            }

            p_s->delta_pic_order_cnt_bottom = (t_sint32)svalue;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: delta_pic_order_cnt_bottom =%li\n",p_s->delta_pic_order_cnt_bottom);
        }
        else
        {
            p_s->delta_pic_order_cnt_bottom = 0;
        }
    }

    /* delta_pic_order_cnt[0] and delta_pic_order_cnt[1] */
    if (p_sp->pic_order_cnt_type==1 && p_sp->delta_pic_order_always_zero_flag==0)
    {
        status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid delta_pic_order_cnt[0]\n", p_s->slice_num);
            return 1;
        }

        p_s->delta_pic_order_cnt[0] = (t_sint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: delta_pic_order_cnt[0] =%li\n",p_s->delta_pic_order_cnt[0]);

        if (p_pp->pic_order_present_flag==1)
        {
            status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);

            if (status!=0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid delta_pic_order_cnt[1]\n", p_s->slice_num);
                return 1;
            }

            p_s->delta_pic_order_cnt[1] = (t_sint32)svalue;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: delta_pic_order_cnt[1] =%li\n",p_s->delta_pic_order_cnt[1]);
        }
        else
        {
            p_s->delta_pic_order_cnt[1] = 0;
        }
    }
    else
    {
        p_s->delta_pic_order_cnt[0] = 0;
        p_s->delta_pic_order_cnt[1] = 0;
    }

    /* redundant_pic_cnt */
    if (p_pp->redundant_pic_cnt_present_flag==1)
    {
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

        if ((status!=0) || (svalue > 127))
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid redundant_pic_cnt\n", p_s->slice_num);
            return 1;
        }

        p_s->redundant_pic_cnt = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: redundant_pic_cnt =%u\n",p_s->redundant_pic_cnt);

        if ((p_s->nut != 5) && (p_s->redundant_pic_cnt==0) && CheckFrameNum(p_s->frame_num, p_s->nri, first_slice, p_old_sl, p_sp, buf))
        {
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid frame_num %i\n", p_s->slice_num, p_s->frame_num);
            p_s->error_type = FRAME_NUM;
#if CONC_FRAME_NUM == 1
            return 1;
#else
            return DROP;
#endif
        }

    }
    else
    {
        p_s->redundant_pic_cnt = 0;
    }

    /* High profile extension */
    if ((p_s->slice_type==1) || (p_s->slice_type==6)) /* B Slices */
    {
      /* direct_spatial_mv_pred_flag */
      if ( ShowBits(p_b,1,&value) )
      {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid direct_spatial_mv_pred_flag\n", p_s->slice_num);
        return 1;
      }
      p_s->direct_spatial_mv_pred_flag = (t_uint16)value;
      (void)FlushBits(p_b,1);
      OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: direct_spatial_mv_pred_flag =%u\n",p_s->direct_spatial_mv_pred_flag);
    }

    if ( ((p_s->slice_type==0) || (p_s->slice_type==5)) || /* <- Baseline Only */
         ((p_s->slice_type==1) || (p_s->slice_type==6)) )  /* <- Main/High Extension */
    {
        /* num_ref_idx_active_override_flag */
        status = ShowBits(p_b,1,&value);
        if (status!=0)
            return 1;
        p_s->num_ref_idx_active_override_flag = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_ref_idx_active_override_flag =%u\n",p_s->num_ref_idx_active_override_flag);

        /* num_ref_idx_l0_active_minus1 */
        if (p_s->num_ref_idx_active_override_flag==1)
        {
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > 15/*p_sp->num_ref_frames*/))
            {
                p_s->error_type = NUMREF;
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid num_ref_idx_l0_active_minus1\n", p_s->slice_num);
                return 1;
            }

            p_s->num_ref_idx_l0_active_minus1 = (t_uint16)svalue;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_ref_idx_l0_active_minus1 =%u\n",p_s->num_ref_idx_l0_active_minus1);
            num_ref_frames = p_s->num_ref_idx_l0_active_minus1 + 1;
            /* Main/High Extension */
            if ((p_s->slice_type==1) || (p_s->slice_type==6)) /* B Slices */
            {
              /* num_ref_idx_l1_active_minus1 */
              status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
              if ( (status!=0) || (svalue > 15))
              {
                p_s->error_type = NUMREF;
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid num_ref_idx_l1_active_minus1\n", p_s->slice_num);
                return 1;
              }
              p_s->num_ref_idx_l1_active_minus1 = (t_uint16)svalue;
              OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_ref_idx_l1_active_minus1 =%u\n",p_s->num_ref_idx_l1_active_minus1);
              num_ref_frames_l1 = p_s->num_ref_idx_l1_active_minus1 + 1;
            }
            else
            {
              p_s->num_ref_idx_l1_active_minus1 = 0;
            }
        }
        else
        {
            num_ref_frames = p_p->num_ref_idx_l0_active_minus1 + 1;
            num_ref_frames_l1 = p_p->num_ref_idx_l1_active_minus1 + 1;
            /* Added By D.F.*/
            p_s->num_ref_idx_l0_active_minus1 = p_p->num_ref_idx_l0_active_minus1;
            p_s->num_ref_idx_l1_active_minus1 = p_p->num_ref_idx_l1_active_minus1;

        }
    }
    else
    {
      /* GG: later this flag could be used uninitialized */
      p_s->num_ref_idx_active_override_flag = 0;
    }

    /* ref_pic_list_reordering() */
    if ( ((p_s->slice_type==0) || (p_s->slice_type==5)) || /* <- Baseline Only */
         ((p_s->slice_type==1) || (p_s->slice_type==6)) )  /* <- Main/High Extension */
    {
        /* ref_pic_list_reordering_flag_l0 */
        status = ShowBits(p_b,1,&value);
        if (status!=0)
            return 1;
        p_s->ref_pic_list_reordering_flag_l0 = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: ref_pic_list_reordering_flag_l0 =%u\n",p_s->ref_pic_list_reordering_flag_l0);

        /* reordering list cycle */
        if (p_s->ref_pic_list_reordering_flag_l0!=0)
        {
            index = 0;
            terminate = 0;
            do
            {
                /* reordering_of_pic_nums_idc */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                if ((status!=0) || ((p_s->svc_extension_flag == -1) && (svalue > 3)) || (svalue > 5) || ((index == 16) && (svalue != 3)))
                {
                    OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: status %d svalue %d index %d\n", status, (int)svalue, (int)index );
                    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid reordering_of_pic_nums_idc_l0\n", p_s->slice_num);
                    return 1;
                }

                p_s->reordering_of_pic_nums_idc[index] = (t_uint16)svalue;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: reordering_of_pic_nums_idc[%d] =%u\n",index,p_s->reordering_of_pic_nums_idc[index]);

                /* abs_diff_pic_num_minus1 */
                if (p_s->reordering_of_pic_nums_idc[index]==0 || p_s->reordering_of_pic_nums_idc[index]==1)
                {
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                    if (status!=0)
                        return 1;
                    p_s->abs_diff_pic_num_minus1[index] = (t_uint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: abs_diff_pic_num_minus1[%d] =%u\n",index,p_s->abs_diff_pic_num_minus1[index]);
                    index++;
                }

                /* long_term_pic_num */
                else if (p_s->reordering_of_pic_nums_idc[index]==2)
                {
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                    if (status!=0)
                        return 1;

                    p_s->long_term_pic_num[index] = (t_uint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: long_term_pic_num[%d] =%u\n",index,p_s->long_term_pic_num[index]);
                    index++;
                }
                /* abs_diff_view_idx_num_minus1 */
                else if ((!p_s->svc_extension_flag)
                         && (p_s->reordering_of_pic_nums_idc[index]==4 || p_s->reordering_of_pic_nums_idc[index]==5))
                {
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                    if (status!=0)
                        return 1;
                    p_s->abs_diff_view_idx_minus1[index] = (t_uint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: abs_diff_view_idx_minus1[%d] =%u\n",index,p_s->abs_diff_view_idx_minus1[index]);
                    index++;
                }
                else
                    terminate = 1;

                if (index > num_ref_frames)
                {
                    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid reordering_of_pic_nums_idc_L0\n", p_s->slice_num);
                    return 1;
                }
            }
            while (terminate==0);
        }
    }

    /* ref_pic_list_reordering() */
    if ( ((p_s->slice_type==1) || (p_s->slice_type==6)) )
    {
        /* ref_pic_list_reordering_flag_l1 */
        status = ShowBits(p_b,1,&value);
        if (status!=0)
            return 1;
        p_s->ref_pic_list_reordering_flag_l1 = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: ref_pic_list_reordering_flag_l1 =%u\n",p_s->ref_pic_list_reordering_flag_l1);

        /* reordering list cycle */
        if (p_s->ref_pic_list_reordering_flag_l1!=0)
        {
            index = 0;
            terminate = 0;
            do
            {
                /* reordering_of_pic_nums_idc */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                if ((status!=0) || ((p_s->svc_extension_flag == -1) && (svalue > 3)) || (svalue > 5) || ((index == 16) && (svalue != 3)))
                {
                    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid reordering_of_pic_nums_idc_l1\n", p_s->slice_num);
                    return 1;
                }

                p_s->reordering_of_pic_nums_idc_l1[index] = (t_uint16)svalue;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: reordering_of_pic_nums_idc_l1[%d] =%u\n",index,p_s->reordering_of_pic_nums_idc[index]);

                /* abs_diff_pic_num_minus1 */
                if (p_s->reordering_of_pic_nums_idc_l1[index]==0 || p_s->reordering_of_pic_nums_idc_l1[index]==1)
                {
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                    if (status!=0)
                        return 1;
                    p_s->abs_diff_pic_num_minus1_l1[index] = (t_uint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: abs_diff_pic_num_minus1_l1[%d] =%u\n",index,p_s->abs_diff_pic_num_minus1_l1[index]);
                    index++;
                }

                /* long_term_pic_num */
                else if (p_s->reordering_of_pic_nums_idc_l1[index]==2)
                {
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                    if (status!=0)
                        return 1;

                    p_s->long_term_pic_num_l1[index] = (t_uint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: long_term_pic_num_l1[%d] =%u\n",index,p_s->long_term_pic_num_l1[index]);
                    index++;
                }
                /* abs_diff_view_idx_num_minus1 */
                else if ((!p_s->svc_extension_flag)
                         && (p_s->reordering_of_pic_nums_idc_l1[index]==4 || p_s->reordering_of_pic_nums_idc_l1[index]==5))
                {
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                    if (status!=0)
                        return 1;
                    p_s->abs_diff_view_idx_minus1_l1[index] = (t_uint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: abs_diff_view_idx_minus1_l1[%d] =%u\n",index,p_s->abs_diff_view_idx_minus1_l1[index]);
                    index++;
                }
                else
                    terminate = 1;

                if (index > num_ref_frames_l1)
                {
                    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid reordering_of_pic_nums_idc_L1\n", p_s->slice_num);
                    return 1;
                }
            }
            while (terminate==0);
        }
    }



    /* weighted prediction ->(Extension for Main/High Profile): not implemented in Baseline */
    if ( ( (p_p->weighted_pred_flag) && ( ((p_s->slice_type==0) || (p_s->slice_type==5))) ) ||
         ( (p_p->weighted_bipred_idc==1) && ((p_s->slice_type==1) || (p_s->slice_type==6)) ) )
    {
      status = pred_weight_table1(p_b,p_s);

      if ( status != 0 )
      {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid parsing in pred_weight_table function\n", p_s->slice_num);
        return 1;
      }
    }

    /* dec_ref_pic_marking() */
    if (p_s->nri != 0)
    {
        if (p_s->nut == 5)
        {
            /* no_output_of_prior_pics_flag and long_term_reference_flag */
            status = ShowBits(p_b,2,&value);
            if (status!=0)
                return 1;
            p_s->no_output_of_prior_pics_flag = (t_uint16)(value>>1);
            p_s->long_term_reference_flag = (t_uint16)(value&0x1);
            (void)FlushBits(p_b,2);
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: no_output_of_prior_pics_flag =%u\n",p_s->no_output_of_prior_pics_flag);
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: long_term_reference_flag =%u\n",p_s->long_term_reference_flag);
        }
        else
        {
            /* adaptive_ref_pic_marking_mode_flag */
            status = ShowBits(p_b,1,&value);
            if (status!=0)
                return 1;
            p_s->adaptive_ref_pic_marking_mode_flag = (t_uint16)value;
            (void)FlushBits(p_b,1);
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: adaptive_ref_pic_marking_mode_flag =%u\n",p_s->adaptive_ref_pic_marking_mode_flag);

            if (p_s->adaptive_ref_pic_marking_mode_flag!=0)
            {
                t_uint16 mmco1to3_flag = 0;
                t_uint16 mmco4_flag = 0;

                index = 0;
                terminate = 0;

                do
                {
                    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                    if ((status!=0) || (svalue > 6))
                    {
                        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid memory_management_control_operation\n", p_s->slice_num);
                        return 1;
                    }

                    p_s->memory_management_control_operation[index] = (t_uint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: memory_management_control_operation[%d] =%u\n",index,p_s->memory_management_control_operation[index]);

                    switch(p_s->memory_management_control_operation[index])
                    {
                        case 1:
                        case 3:
                            if (mmco5_flag)
                            {
                                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid MMCO\n", p_s->slice_num);
                                p_s->error_type = MMCO;
                                return 1;
                            }
                            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                            if (status!=0)
                                return 1;

                            p_s->difference_of_pic_nums_minus1[index] = (t_uint16)svalue;
                            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: difference_of_pic_nums_minus1[%d] =%u\n",index,p_s->difference_of_pic_nums_minus1[index]);

                            if (p_s->memory_management_control_operation[index]==3)
                            {
                                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                                if (status!=0)
                                    return 1;

                                p_s->long_term_frame_idx[index] = (t_uint16)svalue;
                                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: long_term_frame_idx[%d] =%u\n",index,p_s->long_term_frame_idx[index]);
                            }
                            mmco1to3_flag = 1;
                            index++;
                        break;

                        case 2:
                            if (mmco5_flag)
                            {
                                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid MMCO\n", p_s->slice_num);
                                p_s->error_type = MMCO;
                                return 1;
                            }
                            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                            if (status!=0)
                                return 1;

                            p_s->marking_long_term_pic_num[index] = (t_uint16)svalue;
                            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: marking_long_term_pic_num[%d] =%u\n",index,p_s->marking_long_term_pic_num[index]);
                            mmco1to3_flag = 1;
                            index++;
                        break;

                        case 4:
                            if (mmco4_flag)
                            {
                                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid MMCO\n", p_s->slice_num);
                                p_s->error_type = MMCO;
                                return 1;
                            }

                            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                            if (status!=0 || ((t_uint16)svalue > p_sp->num_ref_frames))
                            {
                                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid MMCO 4\n", p_s->slice_num);
                                p_s->error_type = MMCO;
                                return 1;
                            }

                            p_s->max_long_term_frame_idx_plus1[index] = (t_uint16)svalue;
                            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: max_long_term_frame_idx_plus1[%d] =%u\n",index,p_s->max_long_term_frame_idx_plus1[index]);
                            mmco4_flag = 1;
                            index++;
                        break;

                        case 5:
                            if (mmco1to3_flag)
                            {
                                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid MMCO 5\n", p_s->slice_num);
                                p_s->error_type = MMCO;
                                return 1;
                            }
                            mmco5_flag = 1;
                            index++;
                        break;

                        case 6:
                            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
                            if (status!=0)
                                return 1;

                            p_s->long_term_frame_idx[index] = (t_uint16)svalue;
                            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: long_term_frame_idx[%d] =%u\n",index,p_s->long_term_frame_idx[index]);
                            index++;
                        break;

                        case 0:
                            terminate = 1;
                        break;
                    }
                } while (terminate==0);
            }
        }
    }

    p_s->mmco5_flag = mmco5_flag;

   /* D.F. Main/High profile -> cabac_init_idc syntax element*/
    if ( (p_p->entropy_mode_flag) & (p_s->slice_type != 2) & (p_s->slice_type != 7) )
    {
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);
        if (status!=0)  /* D.F. Error concealment -> to be clarified*/
            return 1;
        p_s->cabac_init_idc = (t_uint16)svalue;
    }
    else /* Inferred */
    {
        p_s->cabac_init_idc = 0;
    }

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: cabac_init_idc =%u\n",p_s->cabac_init_idc);

    /* slice_qp_delta */
    status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);

    if (status!=0)
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid slice_qp_delta\n", p_s->slice_num);
        return 1;
    }

    p_s->slice_qp_delta = (t_sint16)svalue;
    p_s->slice_qp = 26 + p_p->pic_init_qp_minus26 + p_s->slice_qp_delta;

    if ((p_s->slice_qp < 0) || (p_s->slice_qp > 51))
    {
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid slice_qp_delta\n", p_s->slice_num);
        return 1;
    }

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: slice_qp_delta =%d\n",p_s->slice_qp_delta);

    /* disable_deblocking_filter_idc, slice_alpha_c0_offset_div2 and slice_beta_offset_div2 */
    if (p_pp->debl_flt_control_present_flag!=0)
    {
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

        if ((status!=0) || (svalue > 2))
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid disable_deblocking_filter\n", p_s->slice_num);
            return 1;
        }

        p_s->disable_deblocking_filter_idc = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: disable_deblocking_filter_idc =%u\n",p_s->disable_deblocking_filter_idc);

        if (p_s->disable_deblocking_filter_idc!=1)
        {
            status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);

            if ((status!=0) || (svalue < -6) || (svalue > 6))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid slice_alpha_c0_offset_div2\n", p_s->slice_num);
                return 1;
            }

            p_s->slice_alpha_c0_offset_div2 = (t_sint16)svalue;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: slice_alpha_c0_offset_div2 =%d\n",p_s->slice_alpha_c0_offset_div2);
            status = GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue);

            if ((status!=0) || (svalue < -6) || (svalue > 6))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in slice %i header: invalid slice_beta_offset_div2\n", p_s->slice_num);
                return 1;
            }

            p_s->slice_beta_offset_div2 = (t_sint16)svalue;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: slice_beta_offset_div2 =%d\n",p_s->slice_beta_offset_div2);
        }
        else
        {
            p_s->slice_alpha_c0_offset_div2 = 0;
            p_s->slice_beta_offset_div2 = 0;
        }
    }
    else
    {
        p_s->disable_deblocking_filter_idc = 0;
        p_s->slice_alpha_c0_offset_div2 = 0;
        p_s->slice_beta_offset_div2 = 0;
    }

    /* slice_group_change_cycle */
    if (p_pp->num_slice_groups_minus1!=0 && p_pp->slice_group_map_type>=3 && p_pp->slice_group_map_type<=5)
    {
        /* Determine ceil(log2(PicSizeInMapUnits/SliceGroupChangeRate + 1)) */
        bits = 15;
        value = ((p_sp->pic_width_in_mbs_minus1+1) * (p_sp->pic_height_in_map_units_minus1+1) / (p_pp->slice_group_change_rate_minus1+1)) + 1;

        if ((((p_sp->pic_width_in_mbs_minus1+1) * (p_sp->pic_height_in_map_units_minus1+1)) % (p_pp->slice_group_change_rate_minus1+1)))
            value++;

        while (((value>>bits)&0x1)==0)
            bits--;
        if ((value&((1<<bits)-1))!=0)
            bits+=1;

        status = ShowBits(p_b,bits,&value);
        if (status!=0)
            return 1;
        (void)FlushBits(p_b,bits);
        p_s->slice_group_change_cycle = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: slice_group_change_cycle =%u\n",p_s->slice_group_change_cycle);
     }

    if (p_s->redundant_pic_cnt > 0)
        res = RS;
    else
        res = OK;

    return res;
}



/**
 * \brief Check correctness of level parsed from SPS
 * \param level Value of the syntax elemet level parsed from SPS
 * \return 1 if error detected, 0 otherwise
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function check the correctness of level parsed from SPS.
 */
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
t_uint16 METH(CheckLevel)(t_uint32 level, t_uint16 profile)
{
    t_uint16 res;

    switch (level)
    {
        case 0:
            if ((profile == 118) || (profile == 128))
                res = 0;  /* MVC case */
            else
                res = 1;
            break;
        case 9: /* MC: added level 1.b (coded as 9) */
        case 10:
        case 11:
        case 12:
        case 13:
        case 20:
        case 21:
        case 22:
        case 30:
        case 31:
        case 32:
        case 40:
		case 41:
        case 42:
        case 50:
		case 51:
            if(dec_static.restrictMaxLevel>=level)
                res = 0;
            else
                res = 1;
        break;

		default:
            res = 1;
        break;
    }

    return res;
}
//-Change end for CR336095,ER334368,ER336290 level 5.1. support


/**
 * \brief Check if first NAL of a primary picture
 * \param p_sh Pointer to slice header structure
 * \param p_old_sl Pointer to old slice parameters structure
 * \param rs_flag Flag signaling redundant slice decoding
 * \return 0 if not first NAL, 1 if first NAL of primary coded picture, 2 if error
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>02-11-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>Modified interface. Added error check.</td></tr>
 * </table>
 * </dl>
 *
 * The function check if the new NAL unit is part of a new primary coded picture. Perform also check on frame_num correctness.
 */

t_uint16 isNewPicture(t_slice_hdr *p_sh, t_old_slice *p_old_sl, t_uint16 rs_flag)
{
    t_uint16 result = 0;
    t_uint16 idr_flag = (p_sh->nut == 5);


    result |= ((p_old_sl->nal_ref_idc != p_sh->nri) && ((p_old_sl->nal_ref_idc == 0) || (p_sh->nri == 0))); /*D.F.*/

    result |= (p_old_sl->idr_flag != idr_flag);

    if ((p_sh->error == 0) || (p_sh->error_type > PIC_PAR_SET_ID))
    {
        result |= (p_old_sl->pps_id != p_sh->pic_par_set_id);
    }
    else
    {
        return result;
    }

    if ((p_sh->error == 0) || (p_sh->error_type > FRAME_NUM))
    {
        result |= (p_old_sl->frame_num != p_sh->frame_num);
    }
    else
    {
        return result;
    }

    if (idr_flag && p_old_sl->idr_flag)
    {
        if (p_old_sl->idr_flag && ((p_sh->error == 0) || (p_sh->error_type > IDR_ID)))
        {
            result |= (p_old_sl->idr_pic_id != p_sh->idr_pic_id);
        }
        else
        {
            return result;
        }
    }

    if (p_sh->error == 0)
    {
        if (p_sh->active_sp->pic_order_cnt_type == 0)
        {
            /* Check pic_order_cnt_lsb to avoid false new frame detection */
            if (!result && (p_old_sl->nal_ref_idc != 0) && (p_old_sl->pic_order_cnt_lsb != p_sh->pic_order_cnt_lsb))
            {
                /* pic_order_cnt_lsb is wrong, correct on the fly */
                p_sh->pic_order_cnt_lsb = p_old_sl->pic_order_cnt_lsb;
            }
            else
            {
                result |= (p_old_sl->pic_order_cnt_lsb != p_sh->pic_order_cnt_lsb);
            }

            result |= (p_old_sl->delta_pic_order_cnt_bottom != p_sh->delta_pic_order_cnt_bottom);
        }

        if (p_sh->active_sp->pic_order_cnt_type == 1)
        {
            result |= (p_old_sl->delta_pic_order_cnt[0] != p_sh->delta_pic_order_cnt[0]);
            result |= (p_old_sl->delta_pic_order_cnt[1] != p_sh->delta_pic_order_cnt[1]);
        }
    }

    if (rs_flag && (p_sh->error == 0))
    {
        result |= (p_sh->redundant_pic_cnt != p_old_sl->redundant_pic_cnt);
    }

#if CONC_FRAME_NUM == 1
    /* Error check */
    if (result)
    {
        if (idr_flag)
        {
            if (p_old_sl->idr_flag && (p_old_sl->idr_pic_id == p_sh->idr_pic_id))
            {
                p_sh->error = 1;
                p_sh->error_type = IDR_ID;

                return 2;
            }
        }
        else
        {
            if ((p_sh->nri > 0) && (p_old_sl->nal_ref_idc > 0) && (p_old_sl->frame_num == p_sh->frame_num))
            {
                p_sh->error = 1;
                p_sh->error_type = FRAME_NUM;

                return 2;
            }
        }
    }
#endif

    return result;
}



/**
 * \brief Check if first NAL of a redundant picture with missing primary picture
 * \param p_sh Pointer to slice header structure
 * \param p_old_sl Pointer to old slice parameters structure
 * \return 0 if not first NAL, 1 if first NAL of redundant slice
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>28-11-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function check if the new NAL unit is a redundant copy of a missing primary picture.
 */

t_uint16 isNewPictureRS(t_slice_hdr *p_sh, t_old_slice *p_old_sl)
{
    t_uint16 result = 0;
    t_uint16 idr_flag = (p_sh->nut == 5);


    result |= (p_old_sl->nal_ref_idc != p_sh->nri);

    result |= (p_old_sl->idr_flag != idr_flag);

    if ((p_sh->error == 0) || (p_sh->error_type > FRAME_NUM))
    {
        result |= (p_old_sl->frame_num != p_sh->frame_num);
    }
    else
    {
        return result;
    }

    if (p_sh->error == 0)
    {
        if (p_sh->active_sp->pic_order_cnt_type == 0)
        {
            result |= (p_old_sl->pic_order_cnt_lsb != p_sh->pic_order_cnt_lsb);
            result |= (p_old_sl->delta_pic_order_cnt_bottom != p_sh->delta_pic_order_cnt_bottom);
        }

        if (p_sh->active_sp->pic_order_cnt_type == 1)
        {
            result |= (p_old_sl->delta_pic_order_cnt[0] != p_sh->delta_pic_order_cnt[0]);
            result |= (p_old_sl->delta_pic_order_cnt[1] != p_sh->delta_pic_order_cnt[1]);
        }
    }

    if (idr_flag && p_old_sl->idr_flag)
    {
        if (p_old_sl->idr_flag && ((p_sh->error == 0) || (p_sh->error_type > IDR_ID)))
        {
            result |= (p_old_sl->idr_pic_id != p_sh->idr_pic_id);
        }
        else
        {
            return result;
        }
    }

    return result;
}



/**
 * \brief Initialize old_slice structure
 * \param old_sl Pointer to old_slice structure containing infos of previous slice
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function initialize old_slice structure.
 */

void InitOldSlice(t_old_slice *old_sl)
{
   old_sl->bottom_field_flag = 0;
   old_sl->frame_num = 0;
   old_sl->nal_ref_idc = 1;
   old_sl->pic_order_cnt_lsb = 0;
   old_sl->delta_pic_order_cnt_bottom = 0;
   old_sl->delta_pic_order_cnt[0] = 1;
   old_sl->delta_pic_order_cnt[1] = 1;
   old_sl->idr_flag = 1;
   old_sl->idr_pic_id = 0;
   old_sl->pps_id = 0;
   old_sl->redundant_pic_cnt = 0;
   old_sl->mmco5_flag = 0;
   old_sl->error_type = 10;
}



/**
 * \brief Save NAL parameters in old_slice structure
 * \param p_sh Pointer to slice header structure
 * \param p_old_sl Pointer to old slice parameters structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>02-11-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>Modified interface.</td></tr>
 * </table>
 * </dl>
 *
 * The function saves some current NAL unit parameters to old_slice structure.
 */

void saveOldPar(t_slice_hdr *p_sh, t_old_slice *p_old_sl)
{
    p_old_sl->nal_ref_idc = p_sh->nri;
    p_old_sl->idr_flag = (p_sh->nut == 5);

    p_old_sl->error_type = p_sh->error_type;

    if ((p_sh->error == 0) || (p_sh->error_type > PIC_PAR_SET_ID))
    {
        p_old_sl->pps_id = p_sh->pic_par_set_id;
    }

    if ((p_sh->error == 0) || (p_sh->error_type > FRAME_NUM))
    {
        p_old_sl->frame_num = p_sh->frame_num;
    }

    if (p_old_sl->idr_flag && ((p_sh->error == 0) || (p_sh->error_type > IDR_ID)))
    {
        p_old_sl->idr_pic_id = p_sh->idr_pic_id;
    }

    if (p_sh->error == 0)
    {
        p_old_sl->pic_order_cnt_lsb = p_sh->pic_order_cnt_lsb;
        p_old_sl->delta_pic_order_cnt_bottom = p_sh->delta_pic_order_cnt_bottom;

        p_old_sl->delta_pic_order_cnt[0] = p_sh->delta_pic_order_cnt[0];
        p_old_sl->delta_pic_order_cnt[1] = p_sh->delta_pic_order_cnt[1];

        p_old_sl->mmco5_flag = p_sh->mmco5_flag;
    }

    if (p_sh->error == 0)
    {
        p_old_sl->redundant_pic_cnt = p_sh->redundant_pic_cnt;
    }
    else
    {
        p_old_sl->redundant_pic_cnt = 0;
    }
}



/**
 * \brief Check if first NAL of a primary picture
 * \param frame_num frame_num parameter as parsed from slice header
 * \param nri nal_ref_idc parameter
 * \param first_slice Flag indicating the presence of a previous parsed slice
 * \param old_sl Pointer to old_slice structure containing infos of previous slice
 * \param p_sp Pointer to active sequnce parameter set structure
 * \param buf Pointer to decoder buffer structure
 * \return 0 if frame_num is correct, 1 otherwise
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function check correctness of frame_num parameter.
 */

t_uint16 CheckFrameNum(t_uint32 frame_num, t_uint16 nri, t_uint16 first_slice, t_old_slice *old_sl, t_seq_par *p_sp, t_dec_buff *buf)
{
#if 0
    t_uint16 i;
#endif
    t_uint32 MaxFrameNum = (1 << (p_sp->log2_max_frame_num_minus4 + 4));
    t_uint16 PreviousFrameNum = old_sl->mmco5_flag ? 0 : old_sl->frame_num;
    t_uint16 prevNum = (PreviousFrameNum + 1) % MaxFrameNum;

    if (old_sl->error_type <= FRAME_NUM)
        return 0;

    if ((old_sl->nal_ref_idc == 0) && (nri == 0))
    {
        if (frame_num != PreviousFrameNum)
            return 1;
    }
    else if (first_slice)
    {
        if ((frame_num == PreviousFrameNum) && (old_sl->nal_ref_idc != 0))
            return 1;
    }

    /* Check for gaps in frame number */
#if 0   // gives problems in case of streaming
    if (((t_uint16)frame_num != PreviousFrameNum) && ((t_uint16)frame_num != prevNum) && (!p_sp->gaps_in_frame_num_value_flag))
    {
#if CONC_FRAME_NUM == 0
        t_uint16 missing = (frame_num < prevNum) ? MaxFrameNum - prevNum + frame_num : frame_num - prevNum;

        /* If the number of missing frames exceed the DPB size then state for an error instead of slice loss */
        if (buf->initialized && (missing > MIN(MaxFrameNum,16)))
            return 1;
#else
        return 1;
#endif
    }
#endif
/* Gives problems in some streams.... */
#if 0
    /* Check if frame_num is already used */
    if (!(old_sl->idr_flag | old_sl->mmco5_flag) && buf->initialized)
    {
        for (i = 0; i < buf->DPBsize+1; i++)
        {
            if (buf->pics_buf[i].marked_short && ((t_uint16)frame_num == buf->pics_buf[i].frame_num))
            {
                return 1;
            }
        }
    }
#endif
    return 0;
}

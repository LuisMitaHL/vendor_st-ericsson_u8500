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
#include "video_components_h264dec_arm_nmf_decoder_src_host_vuiTraces.h"
#endif


#include "settings.h"
#include "host_bitstream.h"
#include "types.h"
#include "host_types.h"
#include "host_sei.h"
#include "common_bitstream.h"



#if ENABLE_VUI == 1

/**
 * \brief Parse VUI messages.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_vui Pointer to the VUI struct
 * \return 0 if successful, 1 otherwise
 * \author Nicola Zandona'
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function parse VUI messages part of SPS.
 */

t_uint16 METH(GetVUI)(t_bit_buffer *p_b, t_vui_data *p_vui)
{
    t_uint32 value;
    t_sint32 svalue;
    t_uint16 status, j;

    /* aspect_ratio_present_flag */
    status = ShowBits(p_b,1,&value);
    if (status!=0)
        return 1;

    p_vui->aspect_ratio_info_present_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: aspect_ratio_info_present_flag =%u\n",p_vui->aspect_ratio_info_present_flag);

    p_vui->sar_width = 1;
    p_vui->sar_height = 1;   /* set it to 1:1 (square pixel) by default */

    if(p_vui->aspect_ratio_info_present_flag)
    {
        /* aspect_ratio_idc */
        status = ShowBits(p_b,8,&value);
        if (status!=0)
            return 1;

        p_vui->aspect_ratio_idc = (t_uint16)value;
        (void)FlushBits(p_b,8);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: aspect_ratio_idc =%u\n",p_vui->aspect_ratio_idc);

        if (p_vui->aspect_ratio_idc == 255)
        {
            status = ShowBits(p_b,16,&value);
            if (status!=0)
                return 1;

            p_vui->sar_width = (t_uint32)value;
            (void)FlushBits(p_b,16);
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: sar_width =%lu\n",p_vui->sar_width);

            status = ShowBits(p_b,16,&value);
            if (status!=0)
                return 1;

            p_vui->sar_height = (t_uint32)value;
            (void)FlushBits(p_b,16);
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: sar_height =%lu\n",p_vui->sar_height);
        }
        else /* added by MC in order to set proper values into sar_width and sar_height to give them back to appli */
        {

        /* Table E-1 : meaning of sample aspect ratio indicator
          0 Unspecified
          1 1:1
          2 12:11
          3 10:11
          4 16:11
          5 40:33
          6 24:11
          7 20:11
          8 32:11
          9 80:33
          10 18:11
          11 15:11
          12 64:33
          13 160:99
          14 4:3
          15 3:2
          16 2:1
          17..254 Reserved
          */

            switch(p_vui->aspect_ratio_idc)
            {
              case 0:
              case 1:
              default:
                p_vui->sar_width = 1;
                p_vui->sar_height = 1;   /* unspecified, set it to 1:1 by default ? */
               break;
              case 2:
                p_vui->sar_width = 12;
                p_vui->sar_height = 11;
                break;
              case 3:
                p_vui->sar_width = 10;
                p_vui->sar_height = 11;
                break;
              case 4:
                p_vui->sar_width = 16;
                p_vui->sar_height = 11;
                break;
              case 5:
                p_vui->sar_width = 40;
                p_vui->sar_height = 33;
                break;
              case 6:
                p_vui->sar_width = 24;
                p_vui->sar_height = 11;
                break;
              case 7:
                p_vui->sar_width = 20;
                p_vui->sar_height = 11;
                break;
              case 8:
                p_vui->sar_width = 32;
                p_vui->sar_height = 11;
                break;
              case 9:
                p_vui->sar_width = 80;
                p_vui->sar_height = 33;
                break;
              case 10:
                p_vui->sar_width = 18;
                p_vui->sar_height = 11;
                break;
              case 11:
                p_vui->sar_width = 15;
                p_vui->sar_height = 11;
                break;
              case 12:
                p_vui->sar_width = 64;
                p_vui->sar_height = 33;
                break;
              case 13:
                p_vui->sar_width = 160;
                p_vui->sar_height = 99;
                break;
              case 14:
                p_vui->sar_width = 4;
                p_vui->sar_height = 3;
                break;
              case 15:
                p_vui->sar_width = 3;
                p_vui->sar_height = 2;
                break;
              case 16:
                p_vui->sar_width = 2;
                p_vui->sar_height = 1;
                break;
            }
        }
    }

    status = ShowBits(p_b,1,&value);
    if (status!=0)
        return 1;

    p_vui->overscan_info_present_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: overscan_info_present_flag =%u\n",p_vui->overscan_info_present_flag);

    if (p_vui->overscan_info_present_flag)
    {
        status = ShowBits(p_b,1,&value);
        if (status!=0)
            return 1;

        p_vui->overscan_appropriate_flag = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: overscan_appropriate_flag =%u\n",p_vui->overscan_appropriate_flag);
    }

    status = ShowBits(p_b,1,&value);
    if (status!=0)
        return 1;

    p_vui->video_signal_type_present_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: video_signal_type_present_flag =%u\n",p_vui->video_signal_type_present_flag);

    if (p_vui->video_signal_type_present_flag)
    {
        status = ShowBits(p_b,3,&value);
        if (status!=0)
            return 1;

        p_vui->video_format = (t_uint16)value;
        (void)FlushBits(p_b,3);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: video_format =%u\n",p_vui->video_format);

        status = ShowBits(p_b,1,&value);
        if (status!=0)
            return 1;

        p_vui->video_full_range_flag = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: video_full_range_flag =%u\n",p_vui->video_full_range_flag);

        status = ShowBits(p_b,1,&value);
        if (status!=0)
            return 1;

        p_vui->colour_description_present_flag = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: colour_description_present_flag =%u\n",p_vui->colour_description_present_flag);

        if (p_vui->colour_description_present_flag)
        {
            if (ShowBits(p_b,8,&value) != 0)
                return 1;

            p_vui->colour_primaries = (t_uint16)value;
            (void)FlushBits(p_b,8);
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: colour_primaries =%u\n",p_vui->colour_primaries);

            if (ShowBits(p_b,8,&value) != 0)
                return 1;

            p_vui->transfer_characteristics = (t_uint16)value;
            (void)FlushBits(p_b,8);
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: transfer_characteristics =%u\n",p_vui->transfer_characteristics);

            if (ShowBits(p_b,8,&value) != 0)
                return 1;

            p_vui->matrix_coefficients = (t_uint16)value;
            (void)FlushBits(p_b,8);
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: matrix_coefficients =%u\n",p_vui->matrix_coefficients);
        }
    }

    if (ShowBits(p_b,1,&value) != 0)
        return 1;

    p_vui->chroma_loc_info_present_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: chroma_loc_info_present_flag =%u\n",p_vui->chroma_loc_info_present_flag);

    if(p_vui->chroma_loc_info_present_flag)
    {
        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->chroma_sample_loc_type_top_field = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: chroma_sample_loc_type_top_field =%u\n",p_vui->chroma_sample_loc_type_top_field);

        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->chroma_sample_loc_type_bottom_field = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: chroma_sample_loc_type_bottom_field =%u\n",p_vui->chroma_sample_loc_type_bottom_field);
    }

    if (ShowBits(p_b,1,&value))
        return 1;

    p_vui->timing_info_present_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: timing_info_present_flag =%u\n",p_vui->timing_info_present_flag);

    if(p_vui->timing_info_present_flag)
    {
        t_uint32 tmp;
        if (ShowBits(p_b,16,&value) != 0)
            return 1;

        tmp = value;
        (void)FlushBits(p_b,16);

        if (ShowBits(p_b,16,&value) != 0)
            return 1;

        p_vui->num_units_in_tick = value + (tmp << 16);
        (void)FlushBits(p_b,16);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_units_in_tick =%lu\n",p_vui->num_units_in_tick);

        if (ShowBits(p_b,16,&value) != 0)
            return 1;

        tmp = value;
        (void)FlushBits(p_b,16);

        if (ShowBits(p_b,16,&value) != 0)
            return 1;

        p_vui->time_scale = value + (tmp << 16);
        (void)FlushBits(p_b,16);

        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: time_scale =%lu\n",p_vui->time_scale);

        if (ShowBits(p_b,1,&value) != 0)
            return 1;

        p_vui->fixed_frame_rate_flag = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: fixed_frame_rate_flag =%u\n",p_vui->fixed_frame_rate_flag);
    }

    /* HRD NAL */

    if (ShowBits(p_b,1,&value) != 0)
        return 1;

    p_vui->nal_hrd_parameters_present_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: nal_hrd_parameters_present_flag =%u\n",p_vui->nal_hrd_parameters_present_flag);

    if(p_vui->nal_hrd_parameters_present_flag)
    {
        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->nal_hrd_parameters.cpb_cnt_minus1 = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: cpb_cnt_minus1 =%u\n",p_vui->nal_hrd_parameters.cpb_cnt_minus1);

        if (ShowBits(p_b,4,&value) != 0)
            return 1;

        p_vui->nal_hrd_parameters.bit_rate_scale = (t_uint16)value;
        (void)FlushBits(p_b,4);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: bit_rate_scale =%u\n",p_vui->nal_hrd_parameters.bit_rate_scale);

        if (ShowBits(p_b,4,&value) != 0)
            return 1;

        p_vui->nal_hrd_parameters.cpb_size_scale = (t_uint16)value;
        (void)FlushBits(p_b,4);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: cpb_size_scale =%u\n",p_vui->nal_hrd_parameters.cpb_size_scale);

        for(j = 0; j < (p_vui->nal_hrd_parameters.cpb_cnt_minus1 + 1); j++)
        {
            if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
                return 1;

            p_vui->nal_hrd_parameters.bit_rate_value_minus1[j] = (t_uint32)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: bit_rate_value_minus1[%d] =%lu\n",j , p_vui->nal_hrd_parameters.bit_rate_value_minus1[j]);

            if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
                return 1;

            p_vui->nal_hrd_parameters.cpb_size_value_minus1[j] = (t_uint32)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: cpb_size_value_minus1[%d] =%lu\n",j , p_vui->nal_hrd_parameters.cpb_size_value_minus1[j]);

            if (ShowBits(p_b,1,&value) != 0)
                return 1;

            p_vui->nal_hrd_parameters.cbr_flag[j] = (t_uint16)value;
            (void)FlushBits(p_b,1);
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: cbr_flag[%d] =%u\n",j, p_vui->nal_hrd_parameters.cbr_flag[j]);
        }

        if (ShowBits(p_b,5,&value) != 0)
            return 1;

        p_vui->nal_hrd_parameters.initial_cpb_removal_delay_length_minus1 = (t_uint16)value;
        (void)FlushBits(p_b,5);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: initial_cpb_removal_delay_length_minus1 =%u\n",p_vui->nal_hrd_parameters.initial_cpb_removal_delay_length_minus1);

        if (ShowBits(p_b,5,&value) != 0)
            return 1;

        p_vui->nal_hrd_parameters.cpb_removal_delay_length_minus1 = (t_uint16)value;
        (void)FlushBits(p_b,5);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: cpb_removal_delay_length_minus1 =%u\n",p_vui->nal_hrd_parameters.cpb_removal_delay_length_minus1);

        if (ShowBits(p_b,5,&value) != 0)
            return 1;

        p_vui->nal_hrd_parameters.dpb_output_delay_length_minus1 = (t_uint16)value;
        (void)FlushBits(p_b,5);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: dpb_output_delay_length_minus1 =%u\n",p_vui->nal_hrd_parameters.dpb_output_delay_length_minus1);

        if (ShowBits(p_b,5,&value) != 0)
            return 1;

        p_vui->nal_hrd_parameters.time_offset_length = (t_uint16)value;
        (void)FlushBits(p_b,5);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: time_offset_length =%u\n",p_vui->nal_hrd_parameters.time_offset_length);
    }


    /* HRD VCL */

    if (ShowBits(p_b,1,&value) != 0)
        return 1;

    p_vui->vcl_hrd_parameters_present_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: vcl_hrd_parameters_present_flag =%u\n",p_vui->vcl_hrd_parameters_present_flag);

    if(p_vui->vcl_hrd_parameters_present_flag)
    {
        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->vcl_hrd_parameters.cpb_cnt_minus1 = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: cpb_cnt_minus1 =%u\n",p_vui->vcl_hrd_parameters.cpb_cnt_minus1);

        if (ShowBits(p_b,4,&value) != 0)
            return 1;

        p_vui->vcl_hrd_parameters.bit_rate_scale = (t_uint16)value;
        (void)FlushBits(p_b,4);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: bit_rate_scale =%u\n",p_vui->vcl_hrd_parameters.bit_rate_scale);

        if (ShowBits(p_b,4,&value) != 0)
            return 1;

        p_vui->vcl_hrd_parameters.cpb_size_scale = (t_uint16)value;
        (void)FlushBits(p_b,4);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: cpb_size_scale =%u\n",p_vui->vcl_hrd_parameters.cpb_size_scale);

        for(j = 0; j < (p_vui->vcl_hrd_parameters.cpb_cnt_minus1 + 1); j++)
        {
            if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
                return 1;

            p_vui->vcl_hrd_parameters.bit_rate_value_minus1[j] = (t_uint32)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: bit_rate_value_minus1[%d] =%lu\n",j , p_vui->vcl_hrd_parameters.bit_rate_value_minus1[j]);

            if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
                return 1;

            p_vui->vcl_hrd_parameters.cpb_size_value_minus1[j] = (t_uint32)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: cpb_size_value_minus1[%d] =%lu\n",j , p_vui->vcl_hrd_parameters.cpb_size_value_minus1[j]);

            if (ShowBits(p_b,1,&value) != 0)
                return 1;

            p_vui->vcl_hrd_parameters.cbr_flag[j] = (t_uint16)value;
            (void)FlushBits(p_b,1);
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: cbr_flag[%d] =%u\n",j, p_vui->vcl_hrd_parameters.cbr_flag[j]);
        }

        if (ShowBits(p_b,5,&value) != 0)
            return 1;

        p_vui->vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1 = (t_uint16)value;
        (void)FlushBits(p_b,5);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: initial_cpb_removal_delay_length_minus1 =%u\n",p_vui->vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1);

        if (ShowBits(p_b,5,&value) != 0)
            return 1;

        p_vui->vcl_hrd_parameters.cpb_removal_delay_length_minus1 = (t_uint16)value;
        (void)FlushBits(p_b,5);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: cpb_removal_delay_length_minus1 =%u\n",p_vui->vcl_hrd_parameters.cpb_removal_delay_length_minus1);

        if (ShowBits(p_b,5,&value) != 0)
            return 1;

        p_vui->vcl_hrd_parameters.dpb_output_delay_length_minus1 = (t_uint16)value;
        (void)FlushBits(p_b,5);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: dpb_output_delay_length_minus1 =%u\n",p_vui->vcl_hrd_parameters.dpb_output_delay_length_minus1);

        if (ShowBits(p_b,5,&value) != 0)
            return 1;

        p_vui->vcl_hrd_parameters.time_offset_length = (t_uint16)value;
        (void)FlushBits(p_b,5);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: time_offset_length =%u\n",p_vui->vcl_hrd_parameters.time_offset_length);
    }

    if(p_vui->nal_hrd_parameters_present_flag || p_vui->vcl_hrd_parameters_present_flag)
    {
        if (ShowBits(p_b,1,&value) != 0)
            return 1;

        p_vui->low_delay_hrd_flag = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: low_delay_hrd_flag =%u\n",p_vui->low_delay_hrd_flag);
    }

    if (ShowBits(p_b,1,&value) != 0)
        return 1;

    p_vui->pic_struct_present_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_struct_present_flag =%u\n",p_vui->pic_struct_present_flag);

    if (ShowBits(p_b,1,&value) != 0)
        return 1;

    p_vui->bitstream_restriction_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: bitstream_restriction_flag =%u\n",p_vui->bitstream_restriction_flag);

    if (p_vui->bitstream_restriction_flag)
    {
        if (ShowBits(p_b,1,&value) != 0)
            return 1;

        p_vui->motion_vectors_over_pic_boundaries_flag = (t_uint16)value;
        (void)FlushBits(p_b,1);
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: motion_vectors_over_pic_boundaries_flag =%u\n",p_vui->motion_vectors_over_pic_boundaries_flag);

        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->max_bytes_per_pic_denom = (t_uint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: max_bytes_per_pic_denom =%lu\n", p_vui->max_bytes_per_pic_denom);

        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->max_bits_per_mb_denom = (t_uint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: max_bits_per_mb_denom =%lu\n", p_vui->max_bits_per_mb_denom);

        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->log2_max_mv_length_horizontal= (t_uint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: log2_max_mv_length_horizontal =%lu\n",p_vui->log2_max_mv_length_horizontal);

        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->log2_max_mv_length_vertical= (t_uint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: log2_max_mv_length_vertical =%lu\n",p_vui->log2_max_mv_length_vertical);

        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->num_reorder_frames= (t_uint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: num_reorder_frames =%lu\n",p_vui->num_reorder_frames);

        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_vui->max_dec_frame_buffering= (t_uint32)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: max_dec_frame_buffering =%lu\n",p_vui->max_dec_frame_buffering);
    }

    return 0;
}


/**
 * \brief Parse MVC VUI messages.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_mvc_vui Pointer to the MVC VUI struct
 * \return 0 if successful, 1 otherwise
 * \author Victor Macela
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function parse MVC VUI messages part of subset SPS.
 */

t_uint16 METH(GetMVCVUI)(t_bit_buffer *p_b, t_mvc_vui_data *p_mvc_vui)
{
    t_uint32 value;
    t_uint32 tmp;
    t_sint32 svalue;
    t_uint16 status;
    t_uint16 i,j;

    /* vui_mvc_num_ops_minus1 */
    status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

    if ((status!=0) || (svalue > (MAX_OPERATION_POINTS - 1)))
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_num_ops_minus1\n");
        return 1;
    }

    p_mvc_vui->vui_mvc_num_ops_minus1 = (t_uint16)svalue;

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_num_ops_minus1 =%u\n",p_mvc_vui->vui_mvc_num_ops_minus1);

    for (i = 0; i <= p_mvc_vui->vui_mvc_num_ops_minus1; i++)
    {
        /* vui_mvc_temporal_id[i] */
        status = ShowBits(p_b,3,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_temporal_id[%u]\n",i);
            return 1;
        }

        p_mvc_vui->vui_mvc_temporal_id[i] = (t_uint16)value;

        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_temporal_id[%u] =%u\n",i,p_mvc_vui->vui_mvc_temporal_id[i]);
        (void)FlushBits(p_b,3);

        /* vui_mvc_num_target_output_views_minus1[i] */
        status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

        if ((status!=0) || (svalue > (MAX_NUM_VIEWS - 1)))
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_num_target_output_views_minus1[%u]\n",i);
            return 1;
        }

        p_mvc_vui->vui_mvc_num_target_output_views_minus1[i] = (t_uint16)svalue;
        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_num_target_output_views_minus1[%u] =%u\n",i,p_mvc_vui->vui_mvc_num_target_output_views_minus1[i]);

        for (j = 0; j <= p_mvc_vui->vui_mvc_num_target_output_views_minus1[i]; j++)
        {
            /* vui_mvc_view_id[i][j] */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > 1023))
            {
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_view_id[%u][%u]\n",i,j);
                return 1;
            }

            p_mvc_vui->vui_mvc_view_id[i][j] = (t_uint16)svalue;
            OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_view_id[%u][%u] =%u\n",i,j,p_mvc_vui->vui_mvc_view_id[i][j]);
        }

        /* vui_mvc_timing_info_present_flag[i] */
        status = ShowBits(p_b,1,&value);

        if (status!=0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_timing_info_present_flag[%u]\n",i);
            return 1;
        }

        p_mvc_vui->vui_mvc_timing_info_present_flag[i] = (t_uint16)value;
        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_timing_info_present_flag[%u] =%u\n",i,p_mvc_vui->vui_mvc_timing_info_present_flag[i]);
        (void)FlushBits(p_b,1);

        if (p_mvc_vui->vui_mvc_timing_info_present_flag[i])
        {
            /* vui_mvc_num_units_in_tick[i] (most significant part) */
            status = ShowBits(p_b,16,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_num_units_in_tick[%u]\n",i);
                return 1;
            }

            tmp = value;

            (void)FlushBits(p_b,16);

            /* vui_mvc_num_units_in_tick[i] (least significant part) */
            status = ShowBits(p_b,16,&value);

            value += tmp << 16;

            if ((status != 0) || (value == 0))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_num_units_in_tick[%u]\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_num_units_in_tick[i] = value;

            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_num_units_in_tick[%u] =%lu\n",i,p_mvc_vui->vui_mvc_num_units_in_tick[i]);
            (void)FlushBits(p_b,16);

            /* vui_mvc_time_scale[i] (most significant part) */
            status = ShowBits(p_b,16,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_time_scale[%u]\n",i);
                return 1;
            }

            tmp = value;

            (void)FlushBits(p_b,16);

            /* vui_mvc_time_scale[i] (least significant part) */
            status = ShowBits(p_b,16,&value);

            value += tmp << 16;

            if ((status != 0) || (value == 0))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_time_scale[%u]\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_time_scale[i] = value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_time_scale[%u] =%lu\n",i,p_mvc_vui->vui_mvc_time_scale[i]);
            (void)FlushBits(p_b,16);

            /* vui_mvc_fixed_frame_rate_flag[i] */
            status = ShowBits(p_b,1,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_fixed_frame_rate_flag[%u]\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_fixed_frame_rate_flag[i] = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_fixed_frame_rate_flag[%u] =%u\n",i,p_mvc_vui->vui_mvc_fixed_frame_rate_flag[i]);
            (void)FlushBits(p_b,1);
        }
        else
        {
            /* Default values when vui_mvc_timing_info_present_flag[i] is equal to 0 */

            /* As no default values are specified in subclause E.2.1 for syntax elements
               num_units_in_tick and mvc_time_scale, 0 is chosen as default value. */
            p_mvc_vui->vui_mvc_num_units_in_tick[i]= 0;
            p_mvc_vui->vui_mvc_time_scale[i] = 0;

            /* Quoted from Annex E - E.2.1 VUI parameters semantics:
               When fixed_frame_rate_flag is not present, it shall be inferred to be equal to 0. */
            p_mvc_vui->vui_mvc_fixed_frame_rate_flag[i] = 0;
        }

        /* vui_mvc_nal_hrd_parameters_present_flag[i] */
        status = ShowBits(p_b,1,&value);

        if (status != 0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters_present_flag[%u]\n",i);
            return 1;
        }

        p_mvc_vui->vui_mvc_nal_hrd_parameters_present_flag[i] = (t_uint16)value;
        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters_present_flag[%u] =%u\n",i,p_mvc_vui->vui_mvc_nal_hrd_parameters_present_flag[i]);
        (void)FlushBits(p_b,1);

        if (p_mvc_vui->vui_mvc_nal_hrd_parameters_present_flag[i])
        {
            /* vui_mvc_nal_hrd_parameters[i].cpb_cnt_minus1 */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > 31))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].cpb_cnt_minus1\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_cnt_minus1 = (t_uint16)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].cpb_cnt_minus1 =%u\n",i,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_cnt_minus1);

            /* vui_mvc_nal_hrd_parameters[i].bit_rate_scale */
            status = ShowBits(p_b,4,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].bit_rate_scale\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].bit_rate_scale = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].bit_rate_scale =%u\n",i,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].bit_rate_scale);
            (void)FlushBits(p_b,4);

            /* vui_mvc_nal_hrd_parameters[i].cpb_size_scale */
            status = ShowBits(p_b,4,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].cpb_size_scale\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_size_scale = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].cpb_size_scale =%u\n",i,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_size_scale);
            (void)FlushBits(p_b,4);

            for (j = 0; j <= p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_cnt_minus1; j++)
            {
                /* vui_mvc_nal_hrd_parameters[i].bit_rate_value_minus1[j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if (status != 0)
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].bit_rate_value_minus1[%u]\n",i,j);
                    return 1;
                }

                p_mvc_vui->vui_mvc_nal_hrd_parameters[i].bit_rate_value_minus1[j] = (t_uint32)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].bit_rate_value_minus1[%u] =%lu\n",i,j,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].bit_rate_value_minus1[j]);

                /* vui_mvc_nal_hrd_parameters[i].cpb_size_value_minus1[j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if (status != 0)
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].cpb_size_value_minus1[%u]\n",i,j);
                    return 1;
                }

                p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_size_value_minus1[j] = (t_uint32)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].cpb_size_value_minus1[%u] =%lu\n",i,j,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_size_value_minus1[j]);

                /* vui_mvc_nal_hrd_parameters[i].cbr_flag[j] */
                status = ShowBits(p_b,1,&value);

                if (status != 0)
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].cbr_flag[%u]\n",i,j);
                    return 1;
                }

                p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cbr_flag[j] = (t_uint16)value;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].cbr_flag[%u] =%u\n",i,j,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cbr_flag[j]);
                (void)FlushBits(p_b,1);
            }

            /* vui_mvc_nal_hrd_parameters[i].initial_cpb_removal_delay_length_minus1 */
            status = ShowBits(p_b,5,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].initial_cpb_removal_delay_length_minus1\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].initial_cpb_removal_delay_length_minus1 = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].initial_cpb_removal_delay_length_minus1 =%u\n",i,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].initial_cpb_removal_delay_length_minus1);
            (void)FlushBits(p_b,5);

            /* vui_mvc_nal_hrd_parameters[i].cpb_removal_delay_length_minus1 */
            status = ShowBits(p_b,5,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].cpb_removal_delay_length_minus1\n",i);
                return 1;
            };

            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_removal_delay_length_minus1 = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].cpb_removal_delay_length_minus1 =%u\n",i,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_removal_delay_length_minus1);
            (void)FlushBits(p_b,5);

            /* vui_mvc_nal_hrd_parameters[i].dpb_output_delay_length_minus1 */
            status = ShowBits(p_b,5,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].dpb_output_delay_length_minus1\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].dpb_output_delay_length_minus1 = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].dpb_output_delay_length_minus1 =%u\n",i,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].dpb_output_delay_length_minus1);
            (void)FlushBits(p_b,5);

            /* vui_mvc_nal_hrd_parameters[i].time_offset_length */
            value = ShowBits(p_b,5,&value);

            if (value != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_nal_hrd_parameters[%u].time_offset_length\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].time_offset_length = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_nal_hrd_parameters[%u].time_offset_length =%u\n",i,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].time_offset_length);
            (void)FlushBits(p_b,5);
        }
        else
        {
            /* Default values when NAL HRD parameters are not present */

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When cpb_cnt_minus1 is not present, it shall be inferred to be equal to 0. */
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_cnt_minus1 = 0;

            /* As no default values are specified in subclause E.2.2 for syntax elements bit_rate_scale
               and cpb_size_scale (respectively used in conjunction with syntax elements
               bit_rate_value_minus1 and cpb_size_value_minus1), 0 is chosen as default value. */
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].bit_rate_scale = 0;
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_size_scale = 0;

            /* When bit_rate_value_minus1 and cpb_size_value_minus1 are not present,
               the inference of their values involves several parameters specified in Annex A.
               As those parameters are not currently available in the code, bit_rate_value_minus1
               and cpb_size_value_minus1 are set equal to 0 while the inference of their values
               according to the standard is left for a next code update. */
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].bit_rate_value_minus1[0] = 0;
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_size_value_minus1[0] = 0;;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the cbr_flag[SchedSelIdx] syntax element is not present,
               the value of cbr_flag shall be inferred to be equal to 0. */
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cbr_flag[0] = 0;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the initial_cpb_removal_delay_length_minus1 syntax element is not present,
               it shall be inferred to be equal to 23. */
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].initial_cpb_removal_delay_length_minus1 = 23;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the cpb_removal_delay_length_minus1 syntax element is not present,
               it shall be inferred to be equal to 23. */
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_removal_delay_length_minus1 = 23;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the dpb_output_delay_length_minus1 syntax element is not present,
               it shall be inferred to be equal to 23. */
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].dpb_output_delay_length_minus1 = 23;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the time_offset_length syntax element is not present,
               it shall be inferred to be equal to 24. */
            p_mvc_vui->vui_mvc_nal_hrd_parameters[i].time_offset_length = 24;
        }

        /* vui_mvc_vcl_hrd_parameters_present_flag[i] */
        status = ShowBits(p_b,1,&value);

        if (status != 0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters_present_flag[%u]\n",i);
            return 1;
        }

        p_mvc_vui->vui_mvc_vcl_hrd_parameters_present_flag[i] = (t_uint16)value;
        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters_present_flag[%u] =%u\n",i,p_mvc_vui->vui_mvc_vcl_hrd_parameters_present_flag[i]);

        (void)FlushBits(p_b,1);

        if (p_mvc_vui->vui_mvc_vcl_hrd_parameters_present_flag[i])
        {
            /* vui_mvc_vcl_hrd_parameters[i].cpb_cnt_minus1 */
            status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

            if ((status!=0) || (svalue > 31))
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].cpb_cnt_minus1\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_cnt_minus1 = (t_uint16)svalue;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].cpb_cnt_minus1 =%u\n",i,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_cnt_minus1);

            /* vui_mvc_vcl_hrd_parameters[i].bit_rate_scale */
            status = ShowBits(p_b,4,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].bit_rate_scale\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].bit_rate_scale = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].bit_rate_scale =%u\n",i,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].bit_rate_scale);
            (void)FlushBits(p_b,4);

            /* vui_mvc_vcl_hrd_parameters[i].cpb_size_scale */
            status = ShowBits(p_b,4,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].cpb_size_scale\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_size_scale = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].cpb_size_scale =%u\n",i,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_size_scale);
            (void)FlushBits(p_b,4);

            for (j = 0; j <= p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_cnt_minus1; j++)
            {
                /* vui_mvc_vcl_hrd_parameters[i].bit_rate_value_minus1[j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if (status != 0)
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].bit_rate_value_minus1[%u]\n",i,j);
                    return 1;
                }

                p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].bit_rate_value_minus1[j] = (t_uint32)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].bit_rate_value_minus1[%u] =%lu\n",i,j,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].bit_rate_value_minus1[j]);

                /* vui_mvc_vcl_hrd_parameters[i].cpb_size_value_minus1[j] */
                status = GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue);

                if (status != 0)
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].cpb_size_value_minus1[%u]\n",i,j);
                    return 1;
                }

                p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_size_value_minus1[j] = (t_uint32)svalue;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].cpb_size_value_minus1[%u] =%lu\n",i,j,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_size_value_minus1[j]);

                /* vui_mvc_vcl_hrd_parameters[i].cbr_flag[j] */
                status = ShowBits(p_b,1,&value);

                if (status != 0)
                {
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].cbr_flag[%u]\n",i,j);
                    return 1;
                }

                p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cbr_flag[j] = (t_uint16)value;
                OstTraceFiltInst3( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].cbr_flag[%u] =%u\n",i,j,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cbr_flag[j]);
                (void)FlushBits(p_b,1);
            }

            /* vui_mvc_vcl_hrd_parameters[i].initial_cpb_removal_delay_length_minus1 */
            status = ShowBits(p_b,5,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].initial_cpb_removal_delay_length_minus1\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].initial_cpb_removal_delay_length_minus1 = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].initial_cpb_removal_delay_length_minus1 =%u\n",i,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].initial_cpb_removal_delay_length_minus1);
            (void)FlushBits(p_b,5);

            /* vui_mvc_vcl_hrd_parameters[i].cpb_removal_delay_length_minus1 */
            status = ShowBits(p_b,5,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].cpb_removal_delay_length_minus1\n",i);
                return 1;
            };

            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_removal_delay_length_minus1 = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].cpb_removal_delay_length_minus1 =%u\n",i,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_removal_delay_length_minus1);
            (void)FlushBits(p_b,5);

            /* vui_mvc_vcl_hrd_parameters[i].dpb_output_delay_length_minus1 */
            status = ShowBits(p_b,5,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].dpb_output_delay_length_minus1\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].dpb_output_delay_length_minus1 = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].dpb_output_delay_length_minus1 =%u\n",i,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].dpb_output_delay_length_minus1);
            (void)FlushBits(p_b,5);

            /* vui_mvc_vcl_hrd_parameters[i].time_offset_length */
            value = ShowBits(p_b,5,&value);

            if (value != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_vcl_hrd_parameters[%u].time_offset_length\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].time_offset_length = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_vcl_hrd_parameters[%u].time_offset_length =%u\n",i,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].time_offset_length);
            (void)FlushBits(p_b,5);
        }
        else
        {
            /* Default values when NAL HRD parameters are not present */

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When cpb_cnt_minus1 is not present, it shall be inferred to be equal to 0. */
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_cnt_minus1 = 0;

            /* As no default values are specified in subclause E.2.2 for syntax elements bit_rate_scale
               and cpb_size_scale (respectively used in conjunction with syntax elements
               bit_rate_value_minus1 and cpb_size_value_minus1), 0 is chosen as default value. */
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].bit_rate_scale = 0;
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_size_scale = 0;

            /* When bit_rate_value_minus1 and cpb_size_value_minus1 are not present,
               the inference of their values involves several parameters specified in Annex A.
               As those parameters are not currently available in the code, bit_rate_value_minus1
               and cpb_size_value_minus1 are set equal to 0 while the inference of their values
               according to the standard is left for a next code update. */
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].bit_rate_value_minus1[0] = 0;
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_size_value_minus1[0] = 0;;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the cbr_flag[SchedSelIdx] syntax element is not present,
               the value of cbr_flag shall be inferred to be equal to 0. */
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cbr_flag[0] = 0;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the initial_cpb_removal_delay_length_minus1 syntax element is not present,
               it shall be inferred to be equal to 23. */
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].initial_cpb_removal_delay_length_minus1 = 23;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the cpb_removal_delay_length_minus1 syntax element is not present,
               it shall be inferred to be equal to 23. */
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_removal_delay_length_minus1 = 23;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the dpb_output_delay_length_minus1 syntax element is not present,
               it shall be inferred to be equal to 23. */
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].dpb_output_delay_length_minus1 = 23;

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When the time_offset_length syntax element is not present,
               it shall be inferred to be equal to 24. */
            p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].time_offset_length = 24;
        }

        if (p_mvc_vui->vui_mvc_nal_hrd_parameters_present_flag[i]
            || p_mvc_vui->vui_mvc_vcl_hrd_parameters_present_flag[i])
        {
            /* vui_mvc_low_delay_hrd_flag[i] */
            status = ShowBits(p_b,1,&value);

            if (status != 0)
            {
                OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_low_delay_hrd_flag[%u]\n",i);
                return 1;
            }

            p_mvc_vui->vui_mvc_low_delay_hrd_flag[i] = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_low_delay_hrd_flag[%u] =%u\n",i,p_mvc_vui->vui_mvc_low_delay_hrd_flag[i]);
            (void)FlushBits(p_b,1);

            /* Quoted from Annex E - E.2.2 HRD parameters semantics:
               When low_delay_hrd_flag is equal to 1, cpb_cnt_minus1 shall be equal to 0. */

            if (p_mvc_vui->vui_mvc_low_delay_hrd_flag[i]
                && (p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_cnt_minus1 != 0))
            {
#if VERB_ERR_CONC == 1
                NMF_LOG("\nSemantics error in MVC VUI parameters extension: vui_mvc_low_delay_hrd_flag[%u] value (%u) "                                "and vui_mvc_nal_hrd_parameters[%u].cpb_cnt_minus1 value (%u) are inconsistent with each other\n",
                       i,p_mvc_vui->vui_mvc_low_delay_hrd_flag[i],i,p_mvc_vui->vui_mvc_nal_hrd_parameters[i].cpb_cnt_minus1);
#endif
                return 1;
            }

            if (p_mvc_vui->vui_mvc_low_delay_hrd_flag[i]
                && (p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_cnt_minus1 != 0))
            {
#if VERB_ERR_CONC == 1
                NMF_LOG("\nSemantics error in MVC VUI parameters extension: vui_mvc_low_delay_hrd_flag[%u] value (%u) "                                "and vui_mvc_vcl_hrd_parameters[%u].cpb_cnt_minus1 value (%u) are inconsistent with each other\n",
                       i,p_mvc_vui->vui_mvc_low_delay_hrd_flag[i],i,p_mvc_vui->vui_mvc_vcl_hrd_parameters[i].cpb_cnt_minus1);
#endif
                return 1;
            }
        }
        else
        {
            /* Quoted from Annex E - E.2.1 VUI parameters semantics:
               When low_delay_hrd_flag is not present, its value shall be inferred
               to be equal to 1 - fixed_frame_rate_flag. */
            p_mvc_vui->vui_mvc_low_delay_hrd_flag[i] = 1 -  p_mvc_vui->vui_mvc_fixed_frame_rate_flag[i];
        }

        /* vui_mvc_pic_struct_present_flag[i] */
        status = ShowBits(p_b,1,&value);

        if (status != 0)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: Syntax error in MVC VUI parameters extension: invalid vui_mvc_pic_struct_present_flag[%u]\n",i);
            return 1;
        }

        p_mvc_vui->vui_mvc_pic_struct_present_flag[i] = (t_uint16)value;
        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: vui_mvc_pic_struct_present_flag[%u] =%u\n",i,p_mvc_vui->vui_mvc_pic_struct_present_flag[i]);
        (void)FlushBits(p_b,1);
    }

    return 0;
}

#endif /* IF ENABLE_VUI == 1 */


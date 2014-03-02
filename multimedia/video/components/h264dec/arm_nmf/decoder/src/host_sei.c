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
#include "video_components_h264dec_arm_nmf_decoder_src_host_seiTraces.h"
#endif

#include "settings.h"
#include "host_bitstream.h"
#include "macros.h"
#include "types.h"
#include "common_bitstream.h"
#include "host_types.h"
#include "host_sei.h"
#include "host_vui.h"
#include "video_generic_chipset_api.h"
extern "C" double ceil(double a);


/* Local functions prototypes */
t_uint16 ByteAlign(t_bit_buffer *p_b);
t_uint16 GetRecoveryPoint(t_bit_buffer *p_b, t_rp_SEI *rp_sei);
t_uint16 GetStereoVideoInfo(t_bit_buffer *p_b,
                            t_stereo_video_info_SEI *p_stereo_video_info_SEI);
t_uint16 GetFramePackingArrangement(t_bit_buffer *p_b,
                                    t_frame_packing_arrangement_SEI *p_frame_packing_arrangement_SEI);
void InterpretFramePackingArrangement(t_frame_packing_arrangement_SEI *p_fpa_SEI,t_dec* dec);
void InterpretStereoVideoInfo(t_stereo_video_info_SEI *p_svi_SEI,t_dec* dec);


#if ENABLE_TIMING == 1
t_uint16 GetBufferingPeriodSEI(t_bit_buffer *p_b, t_dec *dec, t_SEI *p_SEI);
t_uint16 GetPictureTimingSEI(t_bit_buffer *p_b, t_SEI *p_SEI);
#endif

/**
 * \brief Reads a SEI messages NALU type.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_sp Pointer to the sequence parameter set structs array
 * \param p_sei Pointer to SEI struct
 * \return 0 if successful, 1 otherwise
 * \author Nicola Zandona'
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a SEI messages NALU type.
 */

t_uint16 METH(GetSEI)(t_bit_buffer *p_b, t_dec * dec, t_SEI *p_sei)
{
    t_uint16 payload_type, payload_size;
    t_uint32 value;
    t_uint8 tmp_byte;

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: ---- SEI MESSAGES ----");

#if ENABLE_TIMING == 1
    if(dec->is_first_sei_chunk)
    {
      p_sei->timings.bp_flag = 0;
      p_sei->timings.pt_flag = 0;
      dec->is_first_sei_chunk =0;
    }
#endif

    do
    {
        payload_type = 0;

        if (ShowBits(p_b,8,&value) != 0)
            return 1;

        tmp_byte = (t_uint8)value;
        (void)FlushBits(p_b,8);

        while (tmp_byte == 0xFF)
        {
            payload_type += 255;
            if (ShowBits(p_b,8,&value) != 0)
                return 1;

            tmp_byte = (t_uint8)value;
            (void)FlushBits(p_b,8);
        }

        payload_type += tmp_byte;

        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: payload_type =%u", payload_type);

        payload_size = 0;

        if (ShowBits(p_b,8,&value) != 0)
            return 1;

        tmp_byte = (t_uint8)value;
        (void)FlushBits(p_b,8);

        while (tmp_byte == 0xFF)
        {
            payload_size += 255;
            if (ShowBits(p_b,8,&value) != 0)
                return 1;

            tmp_byte = (t_uint8)value;
            (void)FlushBits(p_b,8);
        }

        payload_size += tmp_byte;

        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: payload_size =%u\n", payload_size);

        switch (payload_type)
        {
#if ENABLE_TIMING == 1
            case BP_PAYLOAD_TYPE:
                if(GetBufferingPeriodSEI(p_b, dec, p_sei)!=0)
                  return 1;
                p_sei->timings.bp_flag = 1;
            break;

            case PT_PAYLOAD_TYPE:
                GetPictureTimingSEI(p_b, p_sei);
                p_sei->timings.pt_flag = 1;
            break;
#endif
            case RP_PAYLOAD_TYPE:
                GetRecoveryPoint(p_b, &(p_sei->rp_SEI));
            break;

            case PANSCAN_PAYLOAD_TYPE:
                GetPanScanRectInfo(p_b, &(p_sei->panscan_SEI));
            break;

            case STEREO_VIDEO_INFO_PAYLOAD_TYPE:
                if(GetStereoVideoInfo(p_b, &(p_sei->stereo_video_info_SEI))==0)
                    InterpretStereoVideoInfo(&(p_sei->stereo_video_info_SEI),dec);
                else
                    return 1;
            break;

            case FRAME_PACKING_ARRANGEMENT_PAYLOAD_TYPE:
                if(GetFramePackingArrangement(p_b, &(p_sei->frame_packing_arrangement_SEI))==0)
                    InterpretFramePackingArrangement(&(p_sei->frame_packing_arrangement_SEI),dec);
                else
                    return 1;
            break;

            default:
                OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Payload Type not supported\n");
                (void)FlushBits(p_b, payload_size << 3);
            break;
        }

        if (ShowBits(p_b,8,&value) != 0)
           return 1;

        tmp_byte = (t_uint8)value;

    }
    while (tmp_byte != 0x80);

    return 0;
}



/**
 * \brief Byte aligns the bitstream.
 * \param p_b Pointer to the bit_buffer struct
 * \return 0 if successful, 1 otherwise
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function byte align the bitstream after a SEI message has been parsed.
 */

t_uint16 METH(ByteAlign)(t_bit_buffer *p_b)
{
    t_uint32 value;

    if (p_b->os == 8)
        return 0;

    if (p_b->os != 0)
    {
        if(ShowBits(p_b,1,&value) != 0)
            return 1;

        if (value != 1)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Wrong alignment bit in SEI message\n");
            return 1;
        }

        (void)FlushBits(p_b,1);
    }

    while (p_b->os != 0)
    {
        if(ShowBits(p_b,1,&value) != 0)
            return 1;

        if (value!=0)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Wrong alignment bits in SEI message\n");
            return 1;
        }

        (void)FlushBits(p_b,1);
    }

    return 0;
}


/* Determine the format of current picture based on infos from stereo video info SEI */
void InterpretStereoVideoInfo(t_stereo_video_info_SEI *p_svi_SEI,t_dec* dec)
{
   if(p_svi_SEI->field_views_flag)
   {
       if(p_svi_SEI->top_field_is_left_view_flag)
            dec->buf.curr_3d_format = OMX_3DInterleaveLeftViewFirst;
       else
            dec->buf.curr_3d_format = OMX_3DInterleaveRightViewFirst;
   }
   else
   {
       if(p_svi_SEI->current_frame_is_left_view_flag)
            dec->buf.curr_3d_format = OMX_3DLeftView;
       else
            dec->buf.curr_3d_format = OMX_3DRightView;
   }
}


/* Determine the format of current picture based on infos from frame packing SEI */
void InterpretFramePackingArrangement(t_frame_packing_arrangement_SEI *p_fpa_SEI,t_dec* dec)
{
  if(p_fpa_SEI->frame_packing_arrangement_cancel_flag)
  {
    dec->buf.curr_3d_format =0;
    return;
  }

  switch(p_fpa_SEI->frame_packing_arrangement_type)
  {
    case 2: /* row-interleaved   */
         if(p_fpa_SEI->content_interpretation_type == 2)
            dec->buf.curr_3d_format = OMX_3DInterleaveRightViewFirst;
         else
            dec->buf.curr_3d_format = OMX_3DInterleaveLeftViewFirst;
         break;
    case 3: /* side by side      */
         if(p_fpa_SEI->content_interpretation_type == 2)
            dec->buf.curr_3d_format = OMX_3DSideBySideRightViewFirst;
         else
            dec->buf.curr_3d_format = OMX_3DSideBySideLeftViewFirst;
         break;
    case 4: /* top-bottom        */
         if(p_fpa_SEI->content_interpretation_type == 2)
            dec->buf.curr_3d_format = OMX_3DTopBottomRightViewFirst;
         else
            dec->buf.curr_3d_format = OMX_3DTopBottomLeftViewFirst;
         break;
    case 5: /* temporal itlvd    */
         if(p_fpa_SEI->content_interpretation_type == 2)
         {
              if(p_fpa_SEI->current_frame_is_frame0_flag == 1) dec->buf.curr_3d_format = OMX_3DRightView;
              else                                             dec->buf.curr_3d_format = OMX_3DLeftView;
         }
         else
         {
              if(p_fpa_SEI->current_frame_is_frame0_flag == 1) dec->buf.curr_3d_format = OMX_3DLeftView;
              else                                             dec->buf.curr_3d_format = OMX_3DRightView;
         }
         break;
    case 0: /* checkerboard      */
    case 1: /* column interleave */
    default:
         /* not supported */
         dec->buf.curr_3d_format =0;
         break;
  }
}



/**
 * \brief Reads a Stereo Video Information SEI message.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_sei Pointer to stereo video info SEI struct
 * \return 0 if successful, 1 otherwise
 * \author Victor Macela
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a Stereo Video Information SEI message.
 */

t_uint16 METH(GetStereoVideoInfo)(t_bit_buffer *p_b,
                            t_stereo_video_info_SEI *p_stereo_video_info_SEI)
{
    t_uint32 value;

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST:  ------> Stereo Video Information SEI Message\n");

    if (ShowBits(p_b,1,&value) != 0)
        return 1;

    p_stereo_video_info_SEI->field_views_flag = (t_uint16)value;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: field_views_flag =%u\n", p_stereo_video_info_SEI->field_views_flag);

    (void)FlushBits(p_b,1);

    if (p_stereo_video_info_SEI->field_views_flag)
    {
        if (ShowBits(p_b,1,&value) != 0)
            return 1;

        p_stereo_video_info_SEI->top_field_is_left_view_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: top_field_is_left_view_flag =%u\n", p_stereo_video_info_SEI->top_field_is_left_view_flag);
        (void)FlushBits(p_b,1);
    }
    else
    {
        if (ShowBits(p_b,1,&value) != 0)
            return 1;

        p_stereo_video_info_SEI->current_frame_is_left_view_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: current_frame_is_left_view_flag =%u\n", p_stereo_video_info_SEI->current_frame_is_left_view_flag);
        (void)FlushBits(p_b,1);

        if (ShowBits(p_b,1,&value) != 0)
            return 1;

        p_stereo_video_info_SEI->next_frame_is_second_view_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: next_frame_is_second_view_flag =%u\n", p_stereo_video_info_SEI->next_frame_is_second_view_flag);
        (void)FlushBits(p_b,1);
    }

    if (ShowBits(p_b,1,&value) != 0)
        return 1;

    p_stereo_video_info_SEI->left_view_self_contained_flag = (t_uint16)value;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: left_view_self_contained_flag =%u\n", p_stereo_video_info_SEI->left_view_self_contained_flag);
    (void)FlushBits(p_b,1);

    if (ShowBits(p_b,1,&value) != 0)
        return 1;

    p_stereo_video_info_SEI->right_view_self_contained_flag = (t_uint16)value;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: right_view_self_contained_flag =%u\n", p_stereo_video_info_SEI->right_view_self_contained_flag);
    (void)FlushBits(p_b,1);

    /* Byte Align */
    ByteAlign(p_b);

    return 0;
}
/**
 * \brief Reads a Frame Packing Arrangement SEI message.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_sei Pointer to frame packing arrangement SEI struct
 * \return 0 if successful, 1 otherwise
 * \author Victor Macela
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a Frame Packing Arrangement SEI message.
 */

t_uint16 METH(GetFramePackingArrangement)(t_bit_buffer *p_b,
                                    t_frame_packing_arrangement_SEI *p_frame_packing_arrangement_SEI)
{
    t_uint32 value;
    t_sint32 svalue;

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST:  ------> Frame Packing Arrangement SEI Message\n");

    if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
        return 1;

    p_frame_packing_arrangement_SEI->frame_packing_arrangement_id = (t_uint32)svalue;

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_packing_arrangement_id =%lu\n",p_frame_packing_arrangement_SEI->frame_packing_arrangement_id);

    /* Quoted from Annex D - D.2.25 Frame packing arrangement SEI message semantics:
       The value of frame_packing_arrangement_id shall be in the range of 0 to 2^32 - 2, inclusive.
       Values of frame_packing_arrangement_id from 0 to 255 and from 512 to 2^31 - 1 may be used
       as determined by the application.
       Values of frame_packing_arrangement_id from 256 to 511 and from 2^31 to 2^32 - 2 are reserved
       for future use by ITU-T|ISO/IEC.
       Decoders shall ignore (remove from the bitstream and discard) all frame packing arrangement
       SEI messages containing a value of frame_packing_arrangement_id in the range of 256 to 511
       or in the range of 2^31 to 2^32 - 2, and bitstreams shall not contain such values. */
    if (((p_frame_packing_arrangement_SEI->frame_packing_arrangement_id > 255)
         && (p_frame_packing_arrangement_SEI->frame_packing_arrangement_id < 512))
        || (p_frame_packing_arrangement_SEI->frame_packing_arrangement_id > 2147483647UL))
    {
       OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of frame_packing_arrangement_id is not in the allowed ranges(from 0 to 255 or from 512 to 2^31 - 1)\n");
       return 1;
    }

    if (ShowBits(p_b, 1, &value) != 0)
        return 1;

    p_frame_packing_arrangement_SEI->frame_packing_arrangement_cancel_flag = (t_uint16)value;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_packing_arrangement_cancel_flag =%u\n",p_frame_packing_arrangement_SEI->frame_packing_arrangement_cancel_flag);
    (void)FlushBits(p_b,1);

    if (!p_frame_packing_arrangement_SEI->frame_packing_arrangement_cancel_flag)
    {
        if (ShowBits(p_b, 7, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->frame_packing_arrangement_type = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_packing_arrangement_type =%u\n",p_frame_packing_arrangement_SEI->frame_packing_arrangement_type);
        (void)FlushBits(p_b,7);

        if (ShowBits(p_b, 1, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->quincunx_sampling_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: quincunx_sampling_flag =%u\n",p_frame_packing_arrangement_SEI->quincunx_sampling_flag);

        /* Quoted from Annex D - D.2.25 Frame packing arrangement SEI message semantics:
           When frame_packing_arrangement_type is equal to 0, it is a requirement
           of bitstream conformance to this Specification that quincux_sampling_flag
           shall be equal to 1.
           When frame_packing_arrangement_type is equal to 5, it is a requirement
           of bitstream conformance to this Specification that quincux_sampling_flag
           shall be equal to 0. */
        if ((p_frame_packing_arrangement_SEI->frame_packing_arrangement_type == 0)
            && (p_frame_packing_arrangement_SEI->quincunx_sampling_flag == 0))
        {
              OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of quincux_sampling_flag should be 1 as frame_packing_arrangement_type is equal to 0\n");
              return 1;
        }

        if ((p_frame_packing_arrangement_SEI->frame_packing_arrangement_type == 5)
            && (p_frame_packing_arrangement_SEI->quincunx_sampling_flag == 1))
        {
              OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of quincux_sampling_flag should be 0 as frame_packing_arrangement_type is equal to 5\n");
              return 1;
        }

        (void)FlushBits(p_b,1);

        if (ShowBits(p_b, 6, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->content_interpretation_type = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: content_interpretation_type =%u\n",p_frame_packing_arrangement_SEI->content_interpretation_type);
        (void)FlushBits(p_b,6);

        if (ShowBits(p_b, 1, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->spatial_flipping_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: spatial_flipping_flag =%u\n",p_frame_packing_arrangement_SEI->spatial_flipping_flag);
        (void)FlushBits(p_b,1);

        if (ShowBits(p_b, 1, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->frame0_flipped_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame0_flipped_flag =%u\n", p_frame_packing_arrangement_SEI->frame0_flipped_flag);

        /* Quoted from Annex D - D.2.25:
           When spatial_flipping_flag is equal to 0, it is a requirement of bitstream
           conformance to this Specification that frame0_flipped_flag shall be equal to 0.
           When spatial_flipping_flag is equal to 0, decoders shall ignore the value
           of frame0_flipped_flag. */
        if ((p_frame_packing_arrangement_SEI->spatial_flipping_flag == 0)
            && (p_frame_packing_arrangement_SEI->frame0_flipped_flag == 1))
        {
           OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of frame0_flipped_flag should be 0 as spatial_flipping_flag is equal to 0\n");
        }

        (void)FlushBits(p_b,1);

        if (ShowBits(p_b, 1, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->field_views_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: field_views_flag =%u\n", p_frame_packing_arrangement_SEI->field_views_flag);

        /* Quoted from Annex D - D.2.25:
           When frame_packing_arrangement_type is not equal to 2, it is a requirement
           of bitstream conformance to this Specification that field_views_flag
           shall be equal to 0.
           When frame_packing_arrangement_type is not equal to 2, decoders shall ignore
           the value of field_views_flag. */
        if ((p_frame_packing_arrangement_SEI->frame_packing_arrangement_type != 2)
            && (p_frame_packing_arrangement_SEI->field_views_flag == 1))
        {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of field_views_flag should be 0 as frame_packing_arrangement_type is not equal to 2\n");
        }

        (void)FlushBits(p_b,1);

        if (ShowBits(p_b, 1, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->current_frame_is_frame0_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: current_frame_is_frame0_flag =%u\n",p_frame_packing_arrangement_SEI->current_frame_is_frame0_flag);

        /* Quoted from Annex D - D.2.25 Frame packing arrangement SEI message semantics:
           When frame_packing_arrangement_type is not equal to 5, it is a requirement
           of bitstream conformance to this Specification that current_frame_is_frame0_flag
           shall be equal to 0.
           When frame_packing_arrangement_type is not equal to 5, decoders shall ignore
           the value of current_frame_is_frame0_flag. */
        if ((p_frame_packing_arrangement_SEI->frame_packing_arrangement_type != 5)
            && (p_frame_packing_arrangement_SEI->current_frame_is_frame0_flag == 1))
        {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of current_frame_is_frame0_flag should be 0 as frame_packing_arrangement_type is not equal to 5\n");
        }

        (void)FlushBits(p_b,1);

        if (ShowBits(p_b, 1, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->frame0_self_contained_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame0_self_contained_flag =%u\n",p_frame_packing_arrangement_SEI->frame0_self_contained_flag);

        /* Quoted from Annex D - D.2.25 Frame packing arrangement SEI message semantics:
           When frame_packing_arrangement_type is equal to 0 or 1, it is a requirement
           of bitstream conformance to this Specification that frame0_self_contained_flag
           shall be equal to 0.
           When frame_packing_arrangement_type is equal to 0 or 1, decoders shall ignore
           the value of frame0_self_contained_flag. */
        if (((p_frame_packing_arrangement_SEI->frame_packing_arrangement_type == 0)
             || (p_frame_packing_arrangement_SEI->frame_packing_arrangement_type == 1))
            && (p_frame_packing_arrangement_SEI->frame0_self_contained_flag == 1))
        {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of frame0_self_contained_flag should be 0 as frame_packing_arrangement_type is equal to 0 or 1\n");
        }

        (void)FlushBits(p_b,1);

        if (ShowBits(p_b, 1, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->frame1_self_contained_flag = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame1_self_contained_flag =%u\n",p_frame_packing_arrangement_SEI->frame1_self_contained_flag);

        /* Quoted from Annex D - D.2.25 Frame packing arrangement SEI message semantics:
           When frame_packing_arrangement_type is equal to 0 or 1, it is a requirement
           of bitstream conformance to this Specification that frame1_self_contained_flag
           shall be equal to 0.
           When frame_packing_arrangement_type is equal to 0 or 1, decoders shall ignore
           the value of frame1_self_contained_flag. */
        if (((p_frame_packing_arrangement_SEI->frame_packing_arrangement_type == 0)
             || (p_frame_packing_arrangement_SEI->frame_packing_arrangement_type == 1))
            && (p_frame_packing_arrangement_SEI->frame1_self_contained_flag == 1))
        {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of frame1_self_contained_flag should be 0 as frame_packing_arrangement_type is equal to 0 or 1\n");
        }

        (void)FlushBits(p_b,1);

        if (!p_frame_packing_arrangement_SEI->quincunx_sampling_flag
            && (p_frame_packing_arrangement_SEI->frame_packing_arrangement_type != 5))
        {
            if (ShowBits(p_b, 4, &value) != 0)
                return 1;

            p_frame_packing_arrangement_SEI->frame0_grid_position_x = (t_uint16)value;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame0_grid_position_x =%u\n",p_frame_packing_arrangement_SEI->frame0_grid_position_x);
            (void)FlushBits(p_b,4);

            if (ShowBits(p_b, 4, &value) != 0)
                return 1;

            p_frame_packing_arrangement_SEI->frame0_grid_position_y = (t_uint16)value;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame0_grid_position_y =%u\n",p_frame_packing_arrangement_SEI->frame0_grid_position_y);
            (void)FlushBits(p_b,4);

            if (ShowBits(p_b, 4, &value) != 0)
                return 1;

            p_frame_packing_arrangement_SEI->frame1_grid_position_x = (t_uint16)value;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame1_grid_position_x =%u\n", p_frame_packing_arrangement_SEI->frame1_grid_position_x);
            (void)FlushBits(p_b,4);

            if (ShowBits(p_b, 4, &value) != 0)
                return 1;

            p_frame_packing_arrangement_SEI->frame1_grid_position_y = (t_uint16)value;
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame1_grid_position_y =%u\n", p_frame_packing_arrangement_SEI->frame1_grid_position_y);
            (void)FlushBits(p_b,4);
        }

        if (ShowBits(p_b, 8, &value) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->frame_packing_arrangement_reserved_byte = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_packing_arrangement_reserved_byte =%u\n",p_frame_packing_arrangement_SEI->frame_packing_arrangement_reserved_byte);

        /* Quoted from Annex D - D.2.25 Frame packing arrangement SEI message semantics:
           frame_packing_arrangement_reserved_byte is reserved for future use by ITU-T|ISO/IEC.
           It is a requirement of bitstream conformance to this Specification that the value
           of frame_packing_arrangement_reserved_byte shall be equal to 0.
           All other values of frame_packing_arrangement_reserved_byte are reserved for future use
           by ITU-T|ISO/IEC.
           Decoders shall ignore (remove from the bitstream and discard) the value
           of frame_packing_arrangement_reserved_byte. */
        if (p_frame_packing_arrangement_SEI->frame_packing_arrangement_reserved_byte != 0)
        {
         OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of frame_packing_arrangement_reserved_byte should be 0\n");
        }

        (void)FlushBits(p_b,8);

        if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
            return 1;

        p_frame_packing_arrangement_SEI->frame_packing_arrangement_repetition_period = (t_uint16)svalue;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_packing_arrangement_repetition_period =%u\n",p_frame_packing_arrangement_SEI->frame_packing_arrangement_repetition_period);

        /* Quoted from Annex D - D.2.25 Frame packing arrangement SEI message semantics:
           The value of frame_packing_arrangement_repetition_period shall be in the range
           of 0 to 16384, inclusive. */
        if (p_frame_packing_arrangement_SEI->frame_packing_arrangement_repetition_period > 16384)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of frame_packing_arrangement_repetition_period is not in the allowed range (from 0 to 16384)\n");
            return 1;
        }
    }

    if (ShowBits(p_b, 1, &value) != 0)
        return 1;

    p_frame_packing_arrangement_SEI->frame_packing_arrangement_extension_flag = (t_uint16)value;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: frame_packing_arrangement_extension_flag =%u\n",p_frame_packing_arrangement_SEI->frame_packing_arrangement_extension_flag);

    /* Quoted from Annex D - D.2.25 Frame packing arrangement SEI message semantics:
       frame_packing_arrangement_extension_flag equal to 0 indicates that no additional data
       follows within the frame packing arrangement SEI message.
       It is a requirement of bitstream conformance to this Specification that the value
       of frame_packing_arrangement_extensin_flag shall be equal to 0.
       The value 1 for frame_packing_arrangement_extension_flag is reserved for future use
       by ITU-T|ISO/IEC.
       Decoders shall ignore the value 1 for frame_packing_arrangement_extension_flag
       in a frame packing arrangement SEI message and shall ignore all data that follows
       within a frame packing arrangement SEI message after the value 1
       for frame_packing_arrangement_extension_flag. */
    if (p_frame_packing_arrangement_SEI->frame_packing_arrangement_extension_flag != 0)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: value of frame_packing_arrangement_extension_flag should be 0\n");
    }

    (void)FlushBits(p_b,1);

    /* Byte Align */
    ByteAlign(p_b);

    return 0;
}



/**
 * \brief Reads a pan scan rect info SEI message.
 * \param p_b Pointer to the bit_buffer struct
 * \param rp_sei Pointer to pan scan rect info SEI struct
 * \return 0 if successful, 1 otherwise
 * \author Maurizio Colombo
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a pan scan rect info SEI message.
 */

t_uint16 METH(GetPanScanRectInfo)(t_bit_buffer *p_b, t_panscan_SEI *panscan_sei)
{
    t_uint32 value;
    t_sint32 svalue;

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: ------> Pan Scan Rect Info SEI Message\n");

    if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
        return 1;

    panscan_sei->pan_scan_rect_id = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pan_scan_rect_id =%u\n", panscan_sei->pan_scan_rect_id);

    if (ShowBits(p_b, 1, &value) != 0)
        return 1;

    panscan_sei->pan_scan_rect_cancel_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pan_scan_cancel_flag =%u\n", panscan_sei->pan_scan_rect_cancel_flag);

    if(panscan_sei->pan_scan_rect_cancel_flag == 0)
    {

    if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
        return 1;

    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pan_scan_cnt_minus1 =%d\n", (t_sint16)svalue);

    if(svalue!=0)
    {
      OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST: Only pan_scan_cnt_minus1 equal to zero is allowed in progressive\n");
      return 1;
    }

    if (GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue) != 0)
        return 1;

    panscan_sei->pan_scan_rect_left_offset = (t_sint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pan_scan_rect_left_offset =%d\n", panscan_sei->pan_scan_rect_left_offset);

    if (GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue) != 0)
        return 1;

    panscan_sei->pan_scan_rect_right_offset = (t_sint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pan_scan_rect_left_offset =%d\n", panscan_sei->pan_scan_rect_right_offset);

    if (GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue) != 0)
        return 1;

    panscan_sei->pan_scan_rect_top_offset = (t_sint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pan_scan_rect_left_offset =%d\n", panscan_sei->pan_scan_rect_top_offset);

    if (GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue) != 0)
        return 1;

    panscan_sei->pan_scan_rect_bottom_offset = (t_sint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pan_scan_rect_left_offset =%d\n", panscan_sei->pan_scan_rect_bottom_offset);

    if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
        return 1;

    panscan_sei->pan_scan_rect_repetition_period = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pan_scan_rect_repetition_period =%u\n", panscan_sei->pan_scan_rect_repetition_period);
    } // cancel = 0


    /* Byte Align */
    ByteAlign(p_b);

    return 0;
}




/**
 * \brief Reads a recovery point SEI message.
 * \param p_b Pointer to the bit_buffer struct
 * \param rp_sei Pointer to recovery point SEI struct
 * \return 0 if successful, 1 otherwise
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a recovery point SEI message.
 */

t_uint16 METH(GetRecoveryPoint)(t_bit_buffer *p_b, t_rp_SEI *rp_sei)
{
    t_uint32 value;
    t_sint32 svalue;

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST:  ------> Recovery Point SEI Message\n");

    if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
        return 1;

    rp_sei->recovery_frame_cnt = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: recovery_frame_cnt =%u\n", rp_sei->recovery_frame_cnt);

    if (ShowBits(p_b, 1, &value) != 0)
        return 1;

    rp_sei->exact_match_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: exact_match_flag =%u\n", rp_sei->exact_match_flag);

    if (ShowBits(p_b, 1, &value) != 0)
        return 1;

    rp_sei->broken_link_flag = (t_uint16)value;
    (void)FlushBits(p_b,1);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: broken_link_flag =%u\n", rp_sei->broken_link_flag);

    if (ShowBits(p_b, 2, &value) != 0)
        return 2;

    rp_sei->changing_slice_group_idc = (t_uint16)value;
    (void)FlushBits(p_b,2);
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: changing_slice_group_idc =%u\n", rp_sei->changing_slice_group_idc);

    /* Byte Align */
    ByteAlign(p_b);

    return 0;
}



#if ENABLE_TIMING == 1
/**
 * \brief Reads a Buffering Period SEI message.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_sp Pointer to the sequence parameter set structs array
 * \param p_sei Pointer to SEI struct
 * \return 0 if successful, 1 otherwise
 * \author Nicola Zandona'
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a Buffering Period SEI message.
 */

t_uint16 METH(GetBufferingPeriodSEI)(t_bit_buffer *p_b, t_dec *dec, t_SEI *p_SEI)
{
    t_uint32 value;
    t_sint32 svalue;
    t_uint16 j;
    t_uint16 index_sps = 0;
    t_seq_par * p_sp = dec->stab;
    t_bp_SEI *bp_SEI = &(p_SEI->bp_SEI);

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST:  ------> Buffering Period SEI Message\n");

    if (GetExpGolomb(p_b,EXPGOLOMB_UE,&svalue) != 0)
        return 1;

    bp_SEI->seq_parameter_set_id = (t_uint16)svalue;
    OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: seq_parameter_set_id =%u\n", bp_SEI->seq_parameter_set_id);

    /* check that we don't go beyond num_seq_par to avoid infinite loop */
    while ((p_sp->seq_par_set_id != bp_SEI->seq_parameter_set_id)&&(index_sps++<dec->num_seq_par))
    {
        p_sp++;
    }

    /* if we have overrun num_seq_par, SEI is erroneous so exit */
    if(index_sps>=dec->num_seq_par)
      return 1;

    p_SEI->active_sp = p_sp;

    if (p_sp->vui.nal_hrd_parameters_present_flag)
    {
        for (j = 0; j < (p_sp->vui.nal_hrd_parameters.cpb_cnt_minus1 + 1); j++)
        {
            t_uint16 length = (p_sp->vui.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1 + 1);

            if (ShowBits(p_b,length,&value) != 0)
                return 1;

            bp_SEI->nal_initial_cpb_removal_delay[j] = (t_uint32)value;
            (void)FlushBits(p_b,length);
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: initial_cpb_removal_delay[%d] =%lu\n", j, bp_SEI->nal_initial_cpb_removal_delay[j]);

            if (ShowBits(p_b,length,&value) != 0)
                return 1;

            bp_SEI->nal_initial_cpb_removal_delay_offset[j] = (t_uint32)value;
            (void)FlushBits(p_b,length);
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: initial_cpb_removal_delay_offset[%d] =%lu\n", j, bp_SEI->nal_initial_cpb_removal_delay_offset[j]);
        }
    }

    if (p_sp->vui.vcl_hrd_parameters_present_flag)
    {
        for (j = 0; j< (p_sp->vui.vcl_hrd_parameters.cpb_cnt_minus1 + 1); j++)
        {
            t_uint16 length = (p_sp->vui.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1 + 1);

            if (ShowBits(p_b,length,&value) != 0)
                return 1;

            bp_SEI->vcl_initial_cpb_removal_delay[j] = (t_uint32)value;
            (void)FlushBits(p_b,length);
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: initial_cpb_removal_delay[%d] =%lu\n", j, bp_SEI->vcl_initial_cpb_removal_delay[j]);

            if (ShowBits(p_b,length,&value) != 0)
                return 1;

            bp_SEI->vcl_initial_cpb_removal_delay_offset[j] = (t_uint32)value;
            (void)FlushBits(p_b,length);
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: initial_cpb_removal_delay_offset[%d] =%lu\n", j, bp_SEI->vcl_initial_cpb_removal_delay_offset[j]);
        }
    }

    /* Byte Align */
    ByteAlign(p_b);

    return 0;
}



/**
 * \brief Reads a Picture Timing SEI message.
 * \param p_b Pointer to the bit_buffer struct
 * \param p_sei Pointer to SEI struct
 * \return 0 if successful, 1 otherwise
 * \author Nicola Zandona'
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function reads a Picture Timing SEI message.
 */

t_uint16 METH(GetPictureTimingSEI)(t_bit_buffer *p_b, t_SEI *p_SEI)
{
    t_uint32 value;
    t_sint32 svalue;
    t_uint16 length, j;
    t_uint16 NumClockTS=0;
    t_pt_SEI *pt_SEI = &(p_SEI->pt_SEI);
    t_seq_par *p_sp = p_SEI->active_sp;

    /* The variable code length for these parameters MUST be equal both in NAL HRD parameter and in VCL parameter */
    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: BTST:  ------> Picture Timing SEI Message\n");

    if (p_sp->vui.nal_hrd_parameters_present_flag || p_sp->vui.vcl_hrd_parameters_present_flag)
    {
        length = (p_sp->vui.nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1);
        if (ShowBits(p_b,length,&value) != 0)
            return 1;

        pt_SEI->cpb_removal_delay = (t_uint32)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: cpb_removal_delay =%lu\n", pt_SEI->cpb_removal_delay);
        (void)FlushBits(p_b,length);

        length = (p_sp->vui.nal_hrd_parameters.dpb_output_delay_length_minus1 + 1);
        if (ShowBits(p_b,length,&value) != 0)
            return 1;

        pt_SEI->dpb_output_delay = (t_uint32)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: dpb_output_delay =%lu\n", pt_SEI->dpb_output_delay);
        (void)FlushBits(p_b,length);
    }

    if (p_sp->vui.pic_struct_present_flag)
    {
        if (ShowBits(p_b,4,&value) != 0)
            return 1;

        pt_SEI->pic_struct = (t_uint16)value;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: BTST: pic_struct =%u\n", pt_SEI->pic_struct);
        (void)FlushBits(p_b,4);

        switch (pt_SEI->pic_struct)
        {
            case 0:
            case 1:
            case 2:
                NumClockTS = 1;
            break;

            case 3:
            case 4:
            case 7:
                NumClockTS = 2;
            break;

            case 5:
            case 6:
            case 8:
                NumClockTS = 3;
            break;

            default:
            break;
        }

        for (j = 0; j < NumClockTS; j++)
        {
            if (ShowBits(p_b,1,&value) != 0)
                return 1;

            pt_SEI->clock_timestamp_flag[j] = (t_uint16)value;
            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: clock_timestamp_flag[%d] =%u\n", j, pt_SEI->clock_timestamp_flag[j]);
            (void)FlushBits(p_b,1);

            if(pt_SEI->clock_timestamp_flag[j])
            {
                if (ShowBits(p_b,2,&value) != 0)
                    return 1;

                pt_SEI->ct_type[j] = (t_uint16)value;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: ct_type[%d] =%u\n", j, pt_SEI->ct_type[j]);
                (void)FlushBits(p_b,2);


                if (ShowBits(p_b,1,&value) != 0)
                    return 1;

                pt_SEI->nuit_field_based_flag[j] = (t_uint16)value;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: nuit_field_based_flag[%d] =%u\n", j, pt_SEI->nuit_field_based_flag[j]);
                (void)FlushBits(p_b,1);


                if (ShowBits(p_b,5,&value) != 0)
                    return 1;

                pt_SEI->counting_type[j] = (t_uint16)value;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: counting_type[%d] =%u\n", j, pt_SEI->counting_type[j]);
                (void)FlushBits(p_b,5);


                if (ShowBits(p_b,1,&value) != 0)
                    return 1;

                pt_SEI->full_timestamp_flag[j] = (t_uint16)value;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: full_timestamp_flag[%d] =%u\n", j, pt_SEI->full_timestamp_flag[j]);
                (void)FlushBits(p_b,1);


                if (ShowBits(p_b,1,&value) != 0)
                    return 1;

                pt_SEI->discountinuity_flag[j] = (t_uint16)value;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: discountinuity_flag[%d] =%u\n", j, pt_SEI->discountinuity_flag[j]);
                (void)FlushBits(p_b,1);


                if (ShowBits(p_b,1,&value) != 0)
                    return 1;

                pt_SEI->cnt_dropped_flag[j] = (t_uint16)value;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: cnt_dropped_flag[%d] =%u\n", j, pt_SEI->cnt_dropped_flag[j]);
                (void)FlushBits(p_b,1);


                if (ShowBits(p_b,8,&value))
                    return 1;

                pt_SEI->n_frames[j] = (t_uint16)value;
                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: n_frames[%d] =%u\n", j, pt_SEI->n_frames[j]);
                (void)FlushBits(p_b,8);


                if(pt_SEI->full_timestamp_flag[j])
                {
                    if (ShowBits(p_b,6,&value) != 0)
                        return 1;

                    pt_SEI->seconds_value[j] = (t_uint16)value;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: seconds_value[%d] =%u\n", j, pt_SEI->seconds_value[j]);
                    (void)FlushBits(p_b,6);


                    if (ShowBits(p_b,6,&value) != 0)
                        return 1;

                    pt_SEI->minutes_value[j] = (t_uint16)value;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: minutes_value[%d] =%u\n", j, pt_SEI->minutes_value[j]);
                    (void)FlushBits(p_b,6);


                    if (ShowBits(p_b,5,&value) != 0)
                        return 1;

                    pt_SEI->hours_value[j] = (t_uint16)value;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: hours_value[%d] =%u\n", j, pt_SEI->hours_value[j]);
                    (void)FlushBits(p_b,5);
                }
                else
                {
                    if (ShowBits(p_b,1,&value) != 0)
                        return 1;

                    pt_SEI->seconds_flag[j] = (t_uint16)value;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: seconds_flag[%d] =%u\n", j, pt_SEI->seconds_flag[j]);
                    (void)FlushBits(p_b,1);


                    if(pt_SEI->seconds_flag[j])
                    {
                        if (ShowBits(p_b,6,&value) != 0)
                            return 1;

                        pt_SEI->seconds_value[j] = (t_uint16)value;
                        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: seconds_value[%d] =%u\n", j, pt_SEI->seconds_value[j]);
                        (void)FlushBits(p_b,6);


                        if (ShowBits(p_b,1,&value) != 0)
                            return 1;

                        pt_SEI->minutes_flag[j] = (t_uint16)value;
                        OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: minutes_flag[%d] =%u\n", j, pt_SEI->minutes_flag[j]);
                        (void)FlushBits(p_b,1);


                        if(pt_SEI->minutes_flag[j])
                        {
                            if (ShowBits(p_b,6,&value) != 0)
                                return 1;

                            pt_SEI->minutes_value[j] = (t_uint16)value;
                            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: minutes_value[%d] =%u\n", j, pt_SEI->minutes_value[j]);
                            (void)FlushBits(p_b,6);


                            if (ShowBits(p_b,1,&value) != 0)
                                return 1;

                            pt_SEI->hours_flag[j] = (t_uint16)value;
                            OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: hours_flag[%d] =%u\n", j, pt_SEI->hours_flag[j]);
                            (void)FlushBits(p_b,1);


                            if(pt_SEI->hours_flag[j])
                            {
                                if (ShowBits(p_b,6,&value) != 0)
                                    return 1;

                                pt_SEI->hours_value[j] = (t_uint16)value;
                                OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: hours_value[%d] =%u\n", j, pt_SEI->hours_value[j]);
                                (void)FlushBits(p_b,6);
                            }
                        }
                    }
                }

                length = (p_sp->vui.nal_hrd_parameters.time_offset_length);
                if(length > 0)
                {
                    if (GetExpGolomb(p_b,EXPGOLOMB_SE,&svalue) != 0)
                        return 1;

                    pt_SEI->time_offset[j] = (t_sint16)svalue;
                    OstTraceFiltInst2( TRACE_DEBUG , "H264DEC: BTST: time_offeset[%d] =%u\n", j, pt_SEI->time_offset[j]);
                }
            }
        }
    }

    ByteAlign(p_b);

    return 0;
}



/**
 * \brief Init timings struct.
 * \param p_sei Pointer to SEI struct
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function init timings struct.
 */

void InitTimings(t_SEI *p_SEI)
{
    p_SEI->timings.pt_flag = 0;
    p_SEI->timings.bp_flag = 0;
    p_SEI->timings.nAU = 0;
    p_SEI->timings.bits = 0;
}



/**
 * \brief Decode timings information parsed from VUI/SEI.
 * \param p_sei Pointer to SEI struct
 * \return 0 if successful, 1 otherwise
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-09-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function decode timings information parsed from VUI/SEI. The timings struct will hold CPB removal time and
 * DPB output delay of last decoded AU. This function only works on NAL bitstreams (type II bitstreams) and with
 * only one scheduler (SchedSelIdx always zero).
 */

t_uint16 DecodeTime(t_SEI *p_SEI)
{
    t_timings *timings = &(p_SEI->timings);
    t_hrd_param *hrd_param = &(p_SEI->active_sp->vui.nal_hrd_parameters);

    if(p_SEI->active_sp==NULL) /* MC: robustness */
      return 0;

    if (!timings->pt_flag && !timings->bp_flag)
        return 0;

    if(p_SEI->active_sp->vui.time_scale==0) /* MC: robustness */
     return 0;

    timings->tc = ((double) p_SEI->active_sp->vui.num_units_in_tick / (double) p_SEI->active_sp->vui.time_scale);
    timings->bitrate = (t_uint32) (( hrd_param->bit_rate_value_minus1[0] + 1 ) * ( 1 << (6 + hrd_param->bit_rate_scale)));

    if (timings->nAU == 0)
    {
        /* Initialize HRD */
        timings->previous_cpb_removal_delay = p_SEI->pt_SEI.cpb_removal_delay;
        timings->cpb_removal_delay_correction = 0;
        timings->t_ai = 0;
        timings->t_rn = (double) p_SEI->bp_SEI.nal_initial_cpb_removal_delay[0] / 90000.0;
        timings->t_rn_first = timings->t_rn;
    }
    else
    {
        /*
        Handle the special case of wrap around of cpb_removal_delay.
        The standard specifies that <<The cpb_removal_delay is the remainder of
        a modulo 2(cpb_removal_delay_length_minus1 + 1) counter.>>
        */
        if((p_SEI->pt_SEI.cpb_removal_delay < timings->previous_cpb_removal_delay)
          &&((t_sint32)(timings->previous_cpb_removal_delay - p_SEI->pt_SEI.cpb_removal_delay) >
            (t_sint32)(1<<p_SEI->active_sp->vui.nal_hrd_parameters.cpb_removal_delay_length_minus1)
            )
            )
        {
          timings->cpb_removal_delay_correction +=(1<<p_SEI->active_sp->vui.nal_hrd_parameters.cpb_removal_delay_length_minus1+1);
        }

        timings->t_rn = timings->t_rn_first + (timings->tc *
            (double) (p_SEI->pt_SEI.cpb_removal_delay + timings->cpb_removal_delay_correction));
        timings->previous_cpb_removal_delay = p_SEI->pt_SEI.cpb_removal_delay;

        if (timings->bp_flag)
        {
            timings->t_rn_first = timings->t_rn;
            timings->cpb_removal_delay_correction = 0;
        }

        /* Arrival times */
        if (hrd_param->cbr_flag[0])
        {
            timings->t_ai = timings->t_af;   /* Which is final arrival time of previous AU */
        }
        else
        {
            if (timings->bp_flag)
            {
                timings->t_ai = timings->t_rn - ((double) p_SEI->bp_SEI.nal_initial_cpb_removal_delay[0] / 90000.0);
            }
            else
            {
                timings->t_ai_earliest = timings->t_rn - (((double)(p_SEI->bp_SEI.nal_initial_cpb_removal_delay_offset[0] + (double) p_SEI->bp_SEI.nal_initial_cpb_removal_delay[0])) / 90000.0);

                timings->t_ai = MAX(timings->t_af, timings->t_ai_earliest);
            }
        }
    }

    /* Final arrival time of current AU */
    timings->t_af = timings->t_ai + ((double) timings->bits / (double) timings->bitrate);

    /* Removal time */
    if (p_SEI->active_sp->vui.low_delay_hrd_flag && (timings->t_af > timings->t_rn))
    {
        timings->t_r = timings->t_rn + (timings->tc * ceil( (timings->t_af - timings->t_rn) / timings->tc  ));
    }
    else
    {
        timings->t_r = timings->t_rn;
    }

    /* DPB output time */
    timings->t_o_dpb = timings->t_r + (timings->tc * (double) p_SEI->pt_SEI.dpb_output_delay);

    /* Update AU counter */
    timings->nAU++;

    /* Reset bits counter */
    timings->bits = 0;

#if VERBOSE_STANDARD == 1
    NMF_LOG ("\nDPB output time: %f\n", timings->t_o_dpb);
#endif

    return 1;
}
#endif /* if ENABLE_TIMING == 1 */

#ifdef __SYMBIAN32__
#undef NULL
#include <e32def.h>
#include <e32std.h>
#include <e32debug.h>

extern "C" double ceil(double a)
	{
	int value = (int)a;
	if (value<a && a<=value+1)	return (double)(value+1);
	if (value == a)				return (double)value;
	if (value-1<a && a<=value)	return value;
	__ASSERT_ALWAYS(0,User::Invariant());
	return -1;
	}

#endif




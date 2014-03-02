/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "host_m2d_parser.h"
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg2dec_arm_nmf_parser_src_host_m2d_gethdrTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


/* --- convert codes into n / d values in fps --- */
t_uint16    frame_rate_values[9][2] =
{
    {25, 1},                    /* Forbidden */
    {24000u, 1001u},
    {24, 1},
    {25, 1},
    {30000u, 1001u},
    {30, 1},
    {50, 1},
    {60000u, 1001u},
    {60, 1}
};

/* --- non-linear quantization coefficient table ---------------------------- */
t_uint16    Non_Linear_quantizer_scale[32] =
{
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 10, 12, 14, 16, 18, 20, 22,
    24, 28, 32, 36, 40, 44, 48, 52,
    56, 64, 72, 80, 88, 96, 104, 112
};

/* --- default intra quantization matrix ------------------------------------ */
t_uint16    default_intra_quantizer_matrix[64] =
{
    0x08, 0x10, 0x10, 0x13, 0x10, 0x13, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16, 0x1A, 0x18, 0x1A, 0x1B,
    0x1B, 0x1B, 0x1A, 0x1A, 0x1A, 0x1A, 0x1B, 0x1B,
    0x1B, 0x1D, 0x1D, 0x1D, 0x22, 0x22, 0x22, 0x1D,
    0x1D, 0x1D, 0x1B, 0x1B, 0x1D, 0x1D, 0x20, 0x20,
    0x22, 0x22, 0x25, 0x26, 0x25, 0x23, 0x23, 0x22,
    0x23, 0x26, 0x26, 0x28, 0x28, 0x28, 0x30, 0x30,
    0x2E, 0x2E, 0x38, 0x38, 0x3A, 0x45, 0x45, 0x53
};

t_uint16    default_non_intra_quantizer_matrix[64] =
{
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10
};

/* ==========================================================================
   Name:        Show_Bits
   Description: show next n bits in bitstream.
   ========================================================================== */
t_uint32    mpeg2decParser::Show_Bits (t_bit_buffer *ap_bitstream_buffer, t_uint32 n)
{
	//OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::Show_Bits");
    t_uint32                      nb_of_bytes_to_read;
    t_uint32                      res;
    t_uint8                      *pt_byte_to_read;
    t_uint32                      i;
    t_uint32                      offset_first_byte;
    t_uint32                      offset_last_byte;
    t_uint16                      current_byte;     /* because char >> 8 gives 0xff! */

    offset_first_byte   =   ap_bitstream_buffer->os;
    nb_of_bytes_to_read = ((offset_first_byte + n) >> 3) + 1;
    offset_last_byte    =  (offset_first_byte + n) % 8;

    pt_byte_to_read     = ap_bitstream_buffer->addr;
    res = 0x00000000;

    /* reading of first comple bytes */
    for (i = 0; i < nb_of_bytes_to_read; i++)
    {
        current_byte = *pt_byte_to_read;
        current_byte = current_byte & 0x00ff;
        if (i == 0)
        {
            current_byte = current_byte & (0x00ff >> offset_first_byte);
        }
        if (i != nb_of_bytes_to_read - 1)
            res = (res << 8) | (t_uint32) current_byte;
        else
        {
            current_byte =  current_byte >> (8 - offset_last_byte);
            res          = (res << offset_last_byte) | current_byte;
        }
        pt_byte_to_read++;
    }
	//OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::Show_Bits");
    return res;
}

/* ==========================================================================
   Name:        Flush_Buffer
   Description: advance the Bitstream current bit index and the current byte.
   ========================================================================== */
void        mpeg2decParser::Flush_Buffer (t_bit_buffer *ap_bitstream_buffer, t_uint32 n)
{
	//OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::Flush_Buffer");
    ap_bitstream_buffer->addr += (ap_bitstream_buffer->os + n) >> 3;
    ap_bitstream_buffer->os    = (ap_bitstream_buffer->os + n) %  8;
	//OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::Flush_Buffer");
}

/* ==========================================================================
   Name:        Get_Bits
   Description: get next n bits in the bitstream.
   ========================================================================== */
t_uint32    mpeg2decParser::Get_Bits (t_bit_buffer *ap_bitstream_buffer, t_uint32 n)
{
	//OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::Get_Bits");
    t_uint32                      res;

    res = Show_Bits (ap_bitstream_buffer, n);
    Flush_Buffer    (ap_bitstream_buffer, n);
	//OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::Get_Bits");
	return res;
}

/* ==========================================================================
   Name:        btst_mp2_init
   Description: Init the Bitstream_buffer struct members.
   ========================================================================== */
void        mpeg2decParser::btst_mp2_init (t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::btst_mp2_init");
    ap_bitstream_buffer->os = 0;
    ap_bitstream_buffer->addr = ap_bitstream_buffer->start;
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::btst_mp2_init");
}

/* ==========================================================================
   Name:        Byte_Aligned
   Description: return 1 if bitstream is byte aligned, otherwise return 0
   ========================================================================== */
t_uint32 mpeg2decParser::Byte_Aligned (t_bit_buffer *ap_bitstream_buffer)
{
	//OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::Byte_Aligned");
    if (ap_bitstream_buffer->os == 0) {
		//OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::Byte_Aligned");
        return 1;
	}
    else{
		//OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::Byte_Aligned");
		return 0;
	}

}
/* ==========================================================================
   Name:        next_start_code
   Description: align bitstream on the value of next start code.
   ========================================================================== */
void        mpeg2decParser::next_start_code (t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::next_start_code");
    while (!Byte_Aligned (ap_bitstream_buffer) )
    {
        Get_Bits (ap_bitstream_buffer, 1);
    }
    while ((Get_Bits (ap_bitstream_buffer, 8) != 0) || (Show_Bits (ap_bitstream_buffer, 16) != 1));
    // eat the start code prefix and align on then value
    Get_Bits (ap_bitstream_buffer,16);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::next_start_code");
}

/* ==========================================================================
   Name:        sequence_header
   Description: read sequence header and initialize corresponding variables.
   ========================================================================== */
void        mpeg2decParser::sequence_header (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser:: Parse sequence_header");
    int                           i;
    t_uint32                      vbv_buffer_size_value;

    // resets extension information
    btst_mp2->MPEG2_Flag                  = 0;
    btst_mp2->progressive_sequence        = 1;
    btst_mp2->progressive_frame           = 1;
    btst_mp2->picture_structure           = FRAME_PICTURE;
    btst_mp2->frame_pred_frame_dct        = 1;
    btst_mp2->chroma_format               = CHROMA420;
    btst_mp2->matrix_coefficients         = 1;
    btst_mp2->video_format                = 5;
    btst_mp2->color_primaries             = 1;
    btst_mp2->transfer_characteristics    = 1;
    btst_mp2->f_code[0][0]                = 15;
    btst_mp2->f_code[0][1]                = 15;
    btst_mp2->f_code[1][0]                = 15;
    btst_mp2->f_code[1][1]                = 15;
    btst_mp2->intra_dc_precision          = 0;
    btst_mp2->top_field_first             = 0;
    btst_mp2->concealment_motion_vectors  = 0;
    btst_mp2->q_scale_type                = 0;
    btst_mp2->intra_vlc_format            = 0;
    btst_mp2->alternate_scan              = 0;
    btst_mp2->repeat_first_field          = 0;
    btst_mp2->chroma_420_type             = 1;
    btst_mp2->low_delay                   = 0;
    btst_mp2->composite_display_flag      = 0;
    btst_mp2->scalable_mode               = SC_NONE;

    btst_mp2->horizontal_size             = Get_Bits (ap_bitstream_buffer, 12);
    btst_mp2->vertical_size               = Get_Bits (ap_bitstream_buffer, 12);
    btst_mp2->aspect_ratio_information    = Get_Bits (ap_bitstream_buffer, 4);
    btst_mp2->frame_rate_code             = Get_Bits (ap_bitstream_buffer, 4);
    btst_mp2->bit_rate_value_h            = Get_Bits (ap_bitstream_buffer, 2);
    btst_mp2->bit_rate_value_l            = Get_Bits (ap_bitstream_buffer, 16);

    /* --- ISO/IEC 13818-2 section 6.3.3:  bit_rate is measured in units of 400 bits/second, rounded upwards. */
    btst_mp2->bit_rate                    = ((((t_uint32) btst_mp2->bit_rate_value_h) << 16) | btst_mp2->bit_rate_value_l) * 400;
    btst_mp2->frame_rate                  = frame_rate_values[btst_mp2->frame_rate_code][0] / frame_rate_values[btst_mp2->frame_rate_code][1];
    /* marker bit */                        Get_Bits (ap_bitstream_buffer, 1);
    vbv_buffer_size_value                 = Get_Bits (ap_bitstream_buffer, 10);
    btst_mp2->vbv_buffer_size             = vbv_buffer_size_value*16*1024;
    btst_mp2->constrained_parameters_flag = Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->load_intra_quantizer_matrix = Get_Bits (ap_bitstream_buffer, 1);
    if (btst_mp2->load_intra_quantizer_matrix)
    {
        for (i = 0; i < 64; i++)
        {
            btst_mp2->coded_intra_quantizer_matrix[i] = Get_Bits (ap_bitstream_buffer, 8);
        }
    }
    else
    {
        for (i = 0; i < 64; i++)    /* A remplacer par un memcpy */
        {
            btst_mp2->coded_intra_quantizer_matrix[i] = default_intra_quantizer_matrix[i];
        }
    }
    btst_mp2->load_non_intra_quantizer_matrix = Get_Bits (ap_bitstream_buffer, 1);
    if (btst_mp2->load_non_intra_quantizer_matrix)
    {
        for (i = 0; i < 64; i++)
        {
            btst_mp2->coded_non_intra_quantizer_matrix[i] = Get_Bits (ap_bitstream_buffer, 8);
        }
    }
    else
    {
        for (i = 0; i < 64; i++)    /* A remplacer par un memcpy */
        {
            btst_mp2->coded_non_intra_quantizer_matrix[i] = default_non_intra_quantizer_matrix[i];
        }
    }

    /* round to nearest multiple of coded macroblocks                         */
    /* ISO/IEC 13818-2 section 6.3.3 sequence_header()                        */
    btst_mp2->mb_width  = ((btst_mp2->horizontal_size + 15) >> 4);
    btst_mp2->mb_height = ((btst_mp2->vertical_size   + 15) >> 4);

    /* set default display horizontal and vertical sizes                      */
    btst_mp2->display_horizontal_size = btst_mp2->horizontal_size;
    btst_mp2->display_vertical_size   = btst_mp2->vertical_size;

    /* set window aspect ratio                                                */
//    switch (btst_mp2->aspect_ratio_information)
//    {
//        case 1 : set_mpeg_sar (1, 1)     ;   break;  /* square pixels: SAR = 1*/
//        case 2 : set_mpeg_dar (4, 3)     ;   break;  /* 4/3 format            */
//        case 3 : set_mpeg_dar (16, 9)    ;   break;  /* 16/9 format           */
//        case 4 : set_mpeg_dar (221, 100) ;   break;  /* 2.21 format           */
//        default: set_mpeg_sar (1, 1)     ;   break;  /* unknow                */
//    }
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::sequence_header");
}

/* ==========================================================================
   Name:        group_of_pictures_header
   Description: decode group of pictures header.
   ========================================================================== */
void        mpeg2decParser::group_of_pictures_header (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::group_of_pictures_header");
    btst_mp2->gop_flag    = Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->hour        = Get_Bits (ap_bitstream_buffer, 5);
    btst_mp2->minute      = Get_Bits (ap_bitstream_buffer, 6);
    /* Marker bit */        Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->seconds     = Get_Bits (ap_bitstream_buffer, 6);
    btst_mp2->frame       = Get_Bits (ap_bitstream_buffer, 6);
    btst_mp2->closed_gop  = Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->broken_link = Get_Bits (ap_bitstream_buffer, 1);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::group_of_pictures_header");
}

/* ==========================================================================
   Name:        picture_header
   Description: decode picture header.
   ========================================================================== */
void        mpeg2decParser::picture_header (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::picture_header");
        /* this might be later overwritten by picture_spatial_scalable_extension  */
        btst_mp2->pict_scal           = 0;
        btst_mp2->temporal_reference  = Get_Bits (ap_bitstream_buffer, 10);
        btst_mp2->picture_coding_type = Get_Bits (ap_bitstream_buffer, 3);
        btst_mp2->vbv_delay           = Get_Bits (ap_bitstream_buffer, 16);

        if (btst_mp2->picture_coding_type == I_TYPE)
        {
            btst_mp2->full_pel_forward_vector = 0;
            btst_mp2->forward_f_code          = 7;
        }
        else
        {
            btst_mp2->full_pel_forward_vector = Get_Bits (ap_bitstream_buffer, 1);
            btst_mp2->forward_f_code          = Get_Bits (ap_bitstream_buffer, 3);
        }

        if (btst_mp2->picture_coding_type == B_TYPE)
        {
            btst_mp2->full_pel_backward_vector = Get_Bits (ap_bitstream_buffer, 1);
            btst_mp2->backward_f_code          = Get_Bits (ap_bitstream_buffer, 3);
        }
        else
        {
            btst_mp2->full_pel_backward_vector = 0;
            btst_mp2->backward_f_code          = 7;
        }
        extra_bit_information (btst_mp2, ap_bitstream_buffer);
		OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::picture_header");
}

/* ==========================================================================
   Name:        extra_bit_information
   Description: decode extra bit information.

   ========================================================================== */
t_uint16   mpeg2decParser::extra_bit_information (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::extra_bit_information");
    t_uint16                      Byte_Count = 0;

    while (Get_Bits (ap_bitstream_buffer, 1))
    {
        Get_Bits (ap_bitstream_buffer, 8);
        Byte_Count++;
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::extra_bit_information");
    return Byte_Count;
}

/* ==========================================================================
   Name:        extension_data
   Description: decode extension and user data.
   ========================================================================== */
void       mpeg2decParser::extension_data (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer, t_uint32 *second_field)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::extension_data");
    switch (Get_Bits (ap_bitstream_buffer, 4))
    {
        case SEQUENCE_EXTENSION_ID:
            sequence_extension (btst_mp2, ap_bitstream_buffer);
            break;

        case SEQUENCE_DISPLAY_EXTENSION_ID:
            sequence_display_extension (btst_mp2, ap_bitstream_buffer);
            break;

        case QUANT_MATRIX_EXTENSION_ID:
            quant_matrix_extension (btst_mp2, ap_bitstream_buffer);
            break;

        case SEQUENCE_SCALABLE_EXTENSION_ID:
            sequence_scalable_extension (btst_mp2, ap_bitstream_buffer);
            break;

        case PICTURE_DISPLAY_EXTENSION_ID:
            picture_display_extension (btst_mp2, ap_bitstream_buffer);
            break;

        case PICTURE_CODING_EXTENSION_ID:
            picture_coding_extension (btst_mp2, ap_bitstream_buffer, second_field);
            break;

        case PICTURE_SPATIAL_SCALABLE_EXTENSION_ID:
            picture_spatial_scalable_extension (btst_mp2, ap_bitstream_buffer);
            break;

        case PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID:
            picture_temporal_scalable_extension (btst_mp2, ap_bitstream_buffer);
            break;

        case COPYRIGHT_EXTENSION_ID:
            copyright_extension (btst_mp2, ap_bitstream_buffer);
            break;

        default:
            break;
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::extension_data");
}

/* ==========================================================================
   Name:        sequence_extension
   Description: decode sequence extension.
   ========================================================================== */
void        mpeg2decParser::sequence_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::sequence_extension");
    t_uint16                      horizontal_size_extension;
    t_uint16                      vertical_size_extension;
    t_uint16                      bit_rate_extension;
    t_uint32                      vbv_buffer_size_extension;
    t_uint16                      profile_and_level_indication;

    btst_mp2->MPEG2_Flag           = 1;
    btst_mp2->scalable_mode        = SC_NONE;
    btst_mp2->layer_id             = 0;
    profile_and_level_indication   = Get_Bits (ap_bitstream_buffer, 8);
    btst_mp2->progressive_sequence = Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->chroma_format        = Get_Bits (ap_bitstream_buffer, 2);
    horizontal_size_extension      = Get_Bits (ap_bitstream_buffer, 2);
    vertical_size_extension        = Get_Bits (ap_bitstream_buffer, 2);
    bit_rate_extension             = Get_Bits (ap_bitstream_buffer, 12);
                                     Get_Bits (ap_bitstream_buffer, 1);
    vbv_buffer_size_extension      = Get_Bits (ap_bitstream_buffer, 8);
    btst_mp2->low_delay            = Get_Bits (ap_bitstream_buffer, 1);
    /* frame_rate_extension_n   = */ Get_Bits (ap_bitstream_buffer, 2); /* =0 for Main profile - cf Table 8.5 */
    /* frame_rate_extension_d   = */ Get_Bits (ap_bitstream_buffer, 5); /* =0 for Main profile - cf Table 8.5 */

    /* special case for 422 profile & level must be made */
    if ((profile_and_level_indication >> 7) & 1)
    {
        /* escape bit of profile_and_level_indication set */
        if ((profile_and_level_indication & 15) == 5)
        {
            btst_mp2->profile = PROFILE_422;
            btst_mp2->level   = MAIN_LEVEL;
        }
    }
    else
    {
        btst_mp2->profile     = profile_and_level_indication >> 4;    /* Profile is uper nibble */
        btst_mp2->level       = profile_and_level_indication & 0xF;   /* Level is lower nibble  */
    }
    btst_mp2->horizontal_size   = (horizontal_size_extension << 12) | (btst_mp2->horizontal_size & 0x0fff);
    btst_mp2->vertical_size     = (vertical_size_extension << 12) | (btst_mp2->vertical_size & 0x0fff);

    /* ISO/IEC 13818-2 does not define bit_rate_value to be composed of both  */
    /* the original bit_rate_value parsed in sequence_header() and the        */
    /* optional bit_rate_extension in sequence_extension_header(). However,   */
    /* we use it for bitstream verification purposes.                         */
    btst_mp2->bit_rate_value_h += (bit_rate_extension << 2);
    btst_mp2->bit_rate          = ((((t_uint32) btst_mp2->bit_rate_value_h) << 16) | btst_mp2->bit_rate_value_l) * 400;
    btst_mp2->vbv_buffer_size  += (vbv_buffer_size_extension << 10)*16*1024;

    /* round to nearest multiple of coded macroblocks                         */
    /* ISO/IEC 13818-2 section 6.3.3 sequence_header()                        */
    btst_mp2->mb_width          = (btst_mp2->horizontal_size + 15) / 16;
    btst_mp2->mb_height         = (!btst_mp2->progressive_sequence) ? 2 * ((btst_mp2->vertical_size + 31) / 32) : (btst_mp2->vertical_size + 15) / 16;
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::sequence_extension");
}

/* ==========================================================================
   Name:        sequence_display_extension
   Description: decode sequence display extension.
   ========================================================================== */
void        mpeg2decParser::sequence_display_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::sequence_display_extension");
    t_uint16                      color_description;

    btst_mp2->video_format            = Get_Bits (ap_bitstream_buffer, 3);
    color_description                 = Get_Bits (ap_bitstream_buffer, 1);

    if (color_description)
    {
        btst_mp2->color_primaries          = Get_Bits (ap_bitstream_buffer, 8);
        btst_mp2->transfer_characteristics = Get_Bits (ap_bitstream_buffer, 8);
        btst_mp2->matrix_coefficients      = Get_Bits (ap_bitstream_buffer, 8);
    }

    btst_mp2->display_horizontal_size = Get_Bits (ap_bitstream_buffer, 14);
                                        Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->display_vertical_size   = Get_Bits (ap_bitstream_buffer, 14);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::sequence_display_extension");
}

/* ==========================================================================
   Name:        quant_matrix_extension
   Description: decode quant matrix entension.
   ========================================================================== */
void        mpeg2decParser::quant_matrix_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::quant_matrix_extension");
    int                           i;

    btst_mp2->load_intra_quantizer_matrix     = Get_Bits (ap_bitstream_buffer, 1);
    if (btst_mp2->load_intra_quantizer_matrix)
    {
        for (i = 0; i < 64; i++)
        {
            btst_mp2->coded_intra_quantizer_matrix[i] = Get_Bits (ap_bitstream_buffer, 8);
        }
    }
    btst_mp2->load_non_intra_quantizer_matrix = Get_Bits (ap_bitstream_buffer, 1);
    if (btst_mp2->load_non_intra_quantizer_matrix)
    {
        for (i = 0; i < 64; i++)
        {
            btst_mp2->coded_non_intra_quantizer_matrix[i] = Get_Bits (ap_bitstream_buffer, 8);
        }
    }

    /* Note: ignore chroma quantizer matrix in 420 mode                       */
    if (Get_Bits (ap_bitstream_buffer, 1))
    {
        for (i = 0; i < 64; i++)
        {
            Get_Bits (ap_bitstream_buffer, 8);
        }
    }
    if (Get_Bits (ap_bitstream_buffer, 1))
    {
        for (i = 0; i < 64; i++)
        {
            Get_Bits (ap_bitstream_buffer, 8);
        }
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::quant_matrix_extension");
}

/* ==========================================================================
   Name:        sequence_scalable_extension
   Description: decode sequence scalable extension.
   ========================================================================== */
void        mpeg2decParser::sequence_scalable_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::sequence_scalable_extension");
    /* values (without the +1 offset) of scalable_mode are defined in         */
    /* Table 6-10 of ISO/IEC 13818-2.                                         */
    btst_mp2->scalable_mode = Get_Bits (ap_bitstream_buffer, 2) + 1;
    /* add 1 to make SC_DP != SC_NONE */
    btst_mp2->layer_id      = Get_Bits (ap_bitstream_buffer, 4);

    if (btst_mp2->scalable_mode == SC_SPAT)
    {
        btst_mp2->lower_layer_prediction_horizontal_size = Get_Bits (ap_bitstream_buffer, 14);
                                                           Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->lower_layer_prediction_vertical_size   = Get_Bits (ap_bitstream_buffer, 14);
        btst_mp2->horizontal_subsampling_factor_m        = Get_Bits (ap_bitstream_buffer, 5);
        btst_mp2->horizontal_subsampling_factor_n        = Get_Bits (ap_bitstream_buffer, 5);
        btst_mp2->vertical_subsampling_factor_m          = Get_Bits (ap_bitstream_buffer, 5);
        btst_mp2->vertical_subsampling_factor_n          = Get_Bits (ap_bitstream_buffer, 5);
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::sequence_scalable_extension");
}

/* ==========================================================================
   Name:        picture_display_extension
   Description: decode picture display extension.
   ========================================================================== */
void        mpeg2decParser::picture_display_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::picture_display_extension");
    int                           i;
    t_uint16                      number_of_frame_center_offsets;

    // based on ISO/IEC 13818-2 section 6.3.12 (November 1994)
    // Picture display extensions derive number_of_frame_center_offsets
    if (btst_mp2->progressive_sequence)
    {
        if (btst_mp2->repeat_first_field)
        {
            number_of_frame_center_offsets = btst_mp2->top_field_first ? 3 : 2;
        }
        else
        {
            number_of_frame_center_offsets = 1;
        }
    }
    else
    {
        if (btst_mp2->picture_structure != FRAME_PICTURE)
        {
            number_of_frame_center_offsets = 1;
        }
        else
        {
            number_of_frame_center_offsets = btst_mp2->repeat_first_field ? 3 : 2;
        }
    }
    for (i = 0; i < number_of_frame_center_offsets; i++)
    {
        btst_mp2->frame_center_horizontal_offset[i] = Get_Bits (ap_bitstream_buffer, 16);
                                                      Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->frame_center_vertical_offset[i]   = Get_Bits (ap_bitstream_buffer, 16);
                                                      Get_Bits (ap_bitstream_buffer, 1);
	}
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::picture_display_extension");
}

/* ==========================================================================
  Name:        picture_coding_extension
  Description: decode picture coding extension.
   ========================================================================== */
void       mpeg2decParser::picture_coding_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer, t_uint32 *second_field)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::picture_coding_extension");
#if 0 //#ifdef NO_OMX_BUILD
    //This code is required to be activated in case a single buffer conatains multiple frames
    //So that pointer can go to next frame feild
    if (*second_field == 0)
    {
#endif
        btst_mp2->f_code[0][0]               = Get_Bits (ap_bitstream_buffer, 4);
        btst_mp2->f_code[0][1]               = Get_Bits (ap_bitstream_buffer, 4);
        btst_mp2->f_code[1][0]               = Get_Bits (ap_bitstream_buffer, 4);
        btst_mp2->f_code[1][1]               = Get_Bits (ap_bitstream_buffer, 4);
        btst_mp2->intra_dc_precision         = Get_Bits (ap_bitstream_buffer, 2);
        btst_mp2->picture_structure          = Get_Bits (ap_bitstream_buffer, 2);
        btst_mp2->top_field_first            = Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->frame_pred_frame_dct       = Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->concealment_motion_vectors = Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->q_scale_type               = Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->intra_vlc_format           = Get_Bits (ap_bitstream_buffer, 1) << 4; // 0 or 16
        btst_mp2->alternate_scan             = Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->repeat_first_field         = Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->chroma_420_type            = Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->progressive_frame          = Get_Bits (ap_bitstream_buffer, 1);
        btst_mp2->composite_display_flag     = Get_Bits (ap_bitstream_buffer, 1);

        if (btst_mp2->composite_display_flag)
        {
            btst_mp2->v_axis            = Get_Bits (ap_bitstream_buffer, 1);
            btst_mp2->field_sequence    = Get_Bits (ap_bitstream_buffer, 3);
            btst_mp2->sub_carrier       = Get_Bits (ap_bitstream_buffer, 1);
            btst_mp2->burst_amplitude   = Get_Bits (ap_bitstream_buffer, 7);
            btst_mp2->sub_carrier_phase = Get_Bits (ap_bitstream_buffer, 8);
        }
#if 0 //#ifdef NO_OMX_BUILD
     //This code is required to be activated in case a single buffer conatains multiple frames
     //So that pointer can go to next frame feild
    }
    if (btst_mp2->picture_structure == TOP_FIELD || btst_mp2->picture_structure == BOTTOM_FIELD)
    {
        *second_field = 1;
    }
#endif
		OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::picture_coding_extension");
}

/* ==========================================================================
   Name:        picture_spatial_scalable_extension
   Description: decode picture spatial scalable extension.
   ========================================================================== */
void        mpeg2decParser::picture_spatial_scalable_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::picture_spatial_scalable_extension");
    btst_mp2->pict_scal = 1;    // use spatial scalability in this picture

    btst_mp2->lower_layer_temporal_reference = Get_Bits (ap_bitstream_buffer, 10);
                                               Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->lower_layer_horizontal_offset  = Get_Bits (ap_bitstream_buffer, 15);

    if (btst_mp2->lower_layer_horizontal_offset >= 16384)
    {
        btst_mp2->lower_layer_horizontal_offset -= 32768Ul;
    }
                                               Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->lower_layer_vertical_offset    = Get_Bits (ap_bitstream_buffer, 15);

    if (btst_mp2->lower_layer_vertical_offset >= 16384)
    {
        btst_mp2->lower_layer_vertical_offset -= 32768Ul;
    }
    btst_mp2->spatial_temporal_weight_code_table_index = Get_Bits (ap_bitstream_buffer, 2);
    btst_mp2->lower_layer_progressive_frame            = Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->lower_layer_deinterlaced_field_select    = Get_Bits (ap_bitstream_buffer, 1);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::picture_spatial_scalable_extension");
}

/* ==========================================================================
   Name:        picture_temporal_scalable_extension
   Description: decode picture temporal scalable extension.
   ========================================================================== */
void        mpeg2decParser::picture_temporal_scalable_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::picture_temporal_scalable_extension");
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::picture_temporal_scalable_extension");

}

/* ==========================================================================
   Name:        copyright_extension
   Description: ISO/IEC 13818-2 section 6.2.3.6.-
   (header added in November, 1994 to the IS document).
   ========================================================================== */
void        mpeg2decParser::copyright_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::copyright_extension");
    btst_mp2->copyright_flag       = Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->copyright_identifier = Get_Bits (ap_bitstream_buffer, 8);
    btst_mp2->original_or_copy     = Get_Bits (ap_bitstream_buffer, 1);
    /* reserved_data            = */ Get_Bits (ap_bitstream_buffer, 7);
                                     Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->copyright_number_1_h = Get_Bits (ap_bitstream_buffer, 4);
    btst_mp2->copyright_number_1_l = Get_Bits (ap_bitstream_buffer, 16);
                                     Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->copyright_number_2_h = Get_Bits (ap_bitstream_buffer, 6);
    btst_mp2->copyright_number_2_l = Get_Bits (ap_bitstream_buffer, 16);
                                     Get_Bits (ap_bitstream_buffer, 1);
    btst_mp2->copyright_number_3_h = Get_Bits (ap_bitstream_buffer, 6);
    btst_mp2->copyright_number_3_l = Get_Bits (ap_bitstream_buffer, 16);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::copyright_extension");
}

/* ------------------------------- End of file ---------------------------- */

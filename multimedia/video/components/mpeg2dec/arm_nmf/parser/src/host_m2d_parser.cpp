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
	#include "video_components_mpeg2dec_arm_nmf_parser_src_host_m2d_parserTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

static t_uint32  Second_Field = 0;

/* ==========================================================================
   Name:        VAL_ParseMp2NextFrame
   Description: parse any picture except first of the bitstream 
   ========================================================================== */
t_uint16   mpeg2decParser::VAL_ParseMp2NextFrame (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter  mpeg2decParser::VAL_ParseMp2NextFrame");
    t_uint16    end_of_bitstream_is_reached = 0;
    t_uint16    start_code;

    do
    {   /* locks bitstream on the next sequence start code */
        next_start_code        (ap_bitstream_buffer);
        start_code = Show_Bits (ap_bitstream_buffer, 8);
    }
    while ((start_code != SEQUENCE_HEADER_CODE) &&
           (start_code != GROUP_START_CODE   )  &&
           (start_code != PICTURE_START_CODE)   &&
           (start_code != SEQUENCE_END_CODE));

    while (end_of_bitstream_is_reached != 1)
    {
        start_code = Show_Bits (ap_bitstream_buffer, 8);
        if ((start_code > 0) && (start_code < 0xb0))
        {                       
            /* start of slice, start picture decoding => done by FW         */
            break;
        }
        else
        {
            /* eat the start code */
            Get_Bits (ap_bitstream_buffer, 8);
            switch (start_code)
            {                   /* call header reading function according to the start code */
                case SEQUENCE_HEADER_CODE:
                    sequence_header (btst_mp2, ap_bitstream_buffer);
                    break;
                case GROUP_START_CODE:
                    group_of_pictures_header (btst_mp2, ap_bitstream_buffer);
                    break;
                case EXTENSION_START_CODE:
                    extension_data (btst_mp2, ap_bitstream_buffer,&Second_Field);
                    break;
                case USER_DATA_START_CODE:
                    break;
                case PICTURE_START_CODE:
#if 0 //#ifdef NO_OMX_BUILD
      //This code is required to be activated in case a single buffer conatains multiple frames
      //So that pointer can go to next frame feild
                    if (Second_Field == 0)
                    {
#endif
                        picture_header (btst_mp2, ap_bitstream_buffer);
#if 0 //#ifdef NO_OMX_BUILD
      //This code is required to be activated in case a single buffer conatains multiple frames
      //So that pointer can go to next frame feild

                    }
                    else
                    {
                        do
                        {
                            next_start_code        (ap_bitstream_buffer);
                            start_code = Show_Bits (ap_bitstream_buffer, 8);
                        }
                        while ((start_code != SEQUENCE_HEADER_CODE) &&
                               (start_code != GROUP_START_CODE   )  &&
                               (start_code != PICTURE_START_CODE)   &&
                               (start_code != SEQUENCE_END_CODE));
                        Second_Field                = 0;
                        end_of_bitstream_is_reached = 2; /* avoid to redo next_start_code */
                    }
#endif                    
                    break;
                case SEQUENCE_END_CODE:
                    end_of_bitstream_is_reached = 1;
                    break;
            }
            if (end_of_bitstream_is_reached == 0)
            {
                next_start_code (ap_bitstream_buffer);
            }
#if 0 //#ifdef NO_OMX_BUILD
     //This code is required to be activated in case a single buffer conatains multiple frames
     //So that pointer can go to next frame feild
           
            if (end_of_bitstream_is_reached == 2)
            {
                end_of_bitstream_is_reached = 0;
            }
#endif            
        }
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit  mpeg2decParser::VAL_ParseMp2NextFrame");
    return end_of_bitstream_is_reached;
}

/* ==========================================================================
   Name:        check_mp2_input_parameters
   Description: check result of header parsing and return error code if any 
   ========================================================================== */
OMX_ERRORTYPE  mpeg2decParser::check_mp2_input_parameters(tps_mp2 btst_mp2)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::check_mp2_input_parameters");
    OMX_ERRORTYPE    bitstream_error = OMX_ErrorNone;

    // allow only I, P and B pictures
    if ((btst_mp2->picture_coding_type == 0) || (btst_mp2->picture_coding_type > 3))
    {
        bitstream_error = OMX_ErrorStreamCorrupt;
    }
    // allow only top field, bottom field and frame pictures
    if ((btst_mp2->picture_structure == 0) || (btst_mp2->picture_structure > 3))
    {
        bitstream_error = OMX_ErrorStreamCorrupt;
    } 
    // scalable mode and spatial scalability not supported at MP-ML
    if ((btst_mp2->pict_scal == 1)||(btst_mp2->scalable_mode != SC_NONE))
    {
        OstTraceFiltInst1(TRACE_FLOW,"pict_scal = %d\n",btst_mp2->pict_scal);
        bitstream_error = OMX_ErrorStreamCorrupt;
    }
    OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::check_mp2_input_parameters");
    return (bitstream_error);
}

/* ==========================================================================
   Name:        extract_sequence_parameters
   Description: return reconstructed frame size or 0 if eof bitstream reached
   ========================================================================== */
t_uint32    mpeg2decParser::extract_sequence_parameters (t_bit_buffer *ap_bitstream_buffer)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2decParser::extract_sequence_parameters");
    t_uint16    start_code;
    t_uint32    horizontal_size;
    t_uint32    vertical_size;
    t_uint32    frame_size;
    t_uint32    mb_width;
    t_uint32    mb_height;    
    t_uint16    horizontal_size_extension;
    t_uint16    vertical_size_extension;
    t_uint16    progressive_sequence = 1;

    do
    {   /* locks bitstream on the next sequence start code */
        next_start_code        (ap_bitstream_buffer);
        start_code = Show_Bits (ap_bitstream_buffer, 8);
    }
    while ((start_code != SEQUENCE_HEADER_CODE) && (start_code != SEQUENCE_END_CODE));

    if (start_code == SEQUENCE_END_CODE)
    {
        return (0);
    }
    
    horizontal_size       = Get_Bits (ap_bitstream_buffer, 12);
    vertical_size         = Get_Bits (ap_bitstream_buffer, 12);
                            Get_Bits (ap_bitstream_buffer, 11);
                            Get_Bits (ap_bitstream_buffer, 16);
    /* vbv_buffer_size = */ Get_Bits (ap_bitstream_buffer, 10);
    
    next_start_code        (ap_bitstream_buffer);
    start_code = Get_Bits  (ap_bitstream_buffer, 12);

    //if (start_code == (EXTENSION_START_CODE << 4 | SEQUENCE_EXTENSION_ID))
    if ((start_code & 0xfff) == 0xB51)
    {
        /* MPEG2 stream use case */
                                    Get_Bits (ap_bitstream_buffer, 8);
        progressive_sequence      = Get_Bits (ap_bitstream_buffer, 1);
                                    Get_Bits (ap_bitstream_buffer, 2);
        
        horizontal_size_extension = Get_Bits (ap_bitstream_buffer, 2);
        vertical_size_extension   = Get_Bits (ap_bitstream_buffer, 2);

        horizontal_size = ((horizontal_size_extension <<12) | (horizontal_size & 0x0fff));
        vertical_size   = ((vertical_size_extension   <<12) | (vertical_size & 0x0fff));     
        
    }
    /* round to nearest multiple of coded macroblocks                         */
    /* ISO/IEC 13818-2 section 6.3.3 sequence_header()                        */
    mb_width   = (horizontal_size + 15) >> 4;
    mb_height  = (!progressive_sequence) ? 2 * ((vertical_size + 31) / 32) : (vertical_size + 15) / 16;
    frame_size = (mb_width * mb_height * 3) << 7;
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2decParser::extract_sequence_parameters");
    return (frame_size);
}


/* ------------------------------- End of file ---------------------------- */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_HT_RCV_PARSER_H_
#define _VC1D_HT_RCV_PARSER_H_
/*
 * Includes							       
 */
#include "vc1d_common.h"  /* for types definitions */


#define SC_SEQUENCE_HEADER 0x0FL /**< \brief sequence header start code   */


/********************************************************************
 * RCV constants
 *******************************************************************/

/*
 * The RCV format contains a type byte. The top bit indicates
 * extension data. We need VC1 type, with extension data.
 */
#define RCV_VC1_TYPE (0x85L)
/* Bit 6 of the type indicates V1 if 0, V2 if 1 */
#define RCV_V2_MASK (1UL << 6)
/* Top nibble bits of frame size word are flags in V2 */
#define RCV_V2_FRAMESIZE_FLAGS (0xf0000000L)
#define RCV_V2_KEYFRAME_FLAG   (0x80000000L)

/* V2 extra information has a VBR flag */
#define RCV_V2_VBR_FLAG (0x10000000L)

#define RCV_V1_FILE_HEADER_SIZE 20   /**< \brief rcv file header size is 36 bytes */
#define RCV_V2_DATA_HEADER_SIZE 8   /**< \brief rcv data header size is 8 bytes for V2 */
#define RCV_V1_DATA_HEADER_SIZE 4   /**< \brief rcv data header size is 4 bytes for V1 (no timestamp)*/


typedef struct
{
    t_uint32 frame_size;
    t_uint32 time_stamp;
    t_bool   is_i_frame;
} ts_rcv_frame_data_header, *tps_rcv_frame_data_header;





/*------------------------------------------------------------------------
 * exported functions                                                             
 *----------------------------------------------------------------------*/


extern "C" void rcv_parse_file_header(tps_bitstream_buffer             p_bitstream,
                           tps_decoder_configuration       p_decoder_conf,
                           t_uint8 **p_seq_param,
                           t_uint8 *seq_param_size,
                           t_bool *p_is_rvc2_format);
                           
void rcv_parse_frame_data_header(tps_bitstream_buffer p_bitstream,
                                 tps_rcv_frame_data_header p_frame_header,
                                 t_bool is_rcv2_format);


#endif /* _VC1D_HT_RCV_PARSER_H_ */

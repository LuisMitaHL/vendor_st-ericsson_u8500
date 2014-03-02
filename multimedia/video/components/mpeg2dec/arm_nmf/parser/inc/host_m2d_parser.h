/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _HOST_M2D_PARSER_H_
#define _HOST_M2D_PARSER_H_

//#include "host_decoder.h"   // for mp2 host parameter structure tps_mp2#
#include <inc/type.h>
#include "OMX_Core.h"
#include "armnmf_vdc_mpeg2.idt"
#include "TraceObject.h"
/* --- main known video start codes --------------------------------------- */
#define ERROR                                   (-1)
#define PICTURE_START_CODE                      0x00
#define SLICE_START_CODE_MIN                    0x01
#define SLICE_START_CODE_MAX                    0xaf
#define USER_DATA_START_CODE                    0xb2
#define SEQUENCE_HEADER_CODE                    0xb3
#define SEQUENCE_ERROR_CODE                     0xb4
#define EXTENSION_START_CODE                    0xb5
#define SEQUENCE_END_CODE                       0xb7
#define GROUP_START_CODE                        0xb8
#define SYSTEM_START_CODE_MIN                   0xb9
#define SYSTEM_START_CODE_MAX                   0xff
#define ISO_END_CODE                            0xb9
#define PACK_START_CODE                         0xba
#define SYSTEM_START_CODE                       0xbb
#define VIDEO_ELEMENTARY_STREAM                 0xe0
/* --- scalable_mode ------------------------------------------------------ */
#define SC_NONE                                 0
#define SC_DP                                   1
#define SC_SPAT                                 2
#define SC_SNR                                  3
#define SC_TEMP                                 4
/* --- picture coding type ------------------------------------------------ */
#define I_TYPE                                  1
#define P_TYPE                                  2
#define B_TYPE                                  3
#define D_TYPE                                  4
/* --- picture structure--------------------------------------------------- */
#define TOP_FIELD                               1
#define BOTTOM_FIELD                            2
#define FRAME_PICTURE                           3
#define SCRATCH_PICTURE                         4   /* special debug mode, without any arm processing  */
/* --- chroma_format ------------------------------------------------------ */
#define CHROMA420                               1
#define CHROMA422                               2
#define CHROMA444                               3
/* --- extension start code IDs ------------------------------------------- */
#define SEQUENCE_EXTENSION_ID                   1
#define SEQUENCE_DISPLAY_EXTENSION_ID           2
#define QUANT_MATRIX_EXTENSION_ID               3
#define COPYRIGHT_EXTENSION_ID                  4
#define SEQUENCE_SCALABLE_EXTENSION_ID          5
#define PICTURE_DISPLAY_EXTENSION_ID            7
#define PICTURE_CODING_EXTENSION_ID             8
#define PICTURE_SPATIAL_SCALABLE_EXTENSION_ID   9
#define PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID  10
#define ZIG_ZAG                                 0
#define PROFILE_422                             (128+5)
#define MAIN_LEVEL                              8
class mpeg2decParser: public TraceObject{

	public :
/* --- prototypes of functions ----------------------------- */
		void        btst_mp2_init                       (t_bit_buffer *ap_bitstream_buffer);
		t_uint32 Byte_Aligned (t_bit_buffer *ap_bitstream_buffer);
		
		 t_uint32    Show_Bits                           (t_bit_buffer *ap_bitstream_buffer, t_uint32 n);
		 t_uint32    Get_Bits                            (t_bit_buffer *ap_bitstream_buffer, t_uint32 n);
		 void        Flush_Buffer                        (t_bit_buffer *ap_bitstream_buffer, t_uint32 n);
		 void        next_start_code                     (t_bit_buffer *ap_bitstream_buffer);
		
		void        sequence_header                     (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        group_of_pictures_header            (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        picture_header                      (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        extension_data                      (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer, t_uint32 *second_field);
		t_uint16    extra_bit_information               (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        sequence_extension                  (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        sequence_display_extension          (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        quant_matrix_extension              (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        sequence_scalable_extension         (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        picture_display_extension           (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        picture_coding_extension            (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer, t_uint32 *second_field);
		void        picture_spatial_scalable_extension  (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        picture_temporal_scalable_extension (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		void        copyright_extension                 (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		
		t_uint16    VAL_ParseMp2NextFrame               (tps_mp2 btst_mp2, t_bit_buffer *ap_bitstream_buffer);
		OMX_ERRORTYPE    check_mp2_input_parameters          (tps_mp2 btst_mp2);
		t_uint32    extract_sequence_parameters         (t_bit_buffer *ap_bitstream_buffer);
};

#endif /* _HOST_M2D_PARSER_H_ */

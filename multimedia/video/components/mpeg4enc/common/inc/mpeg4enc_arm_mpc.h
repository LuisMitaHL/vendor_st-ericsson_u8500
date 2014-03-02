/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __MPEG4ENC_ARM_MPC_H
#define __MPEG4ENC_ARM_MPC_H

#include <stdio.h>

#ifdef __MPEG4ENC_ARM_NMF //for ARM-NMF
	#include <string.h>
	#include "Component.h"
	#include "ENS_List.h"
	#include <t1xhv_common.idt>
	#include <t1xhv_vec.idt>
	#include <vfm_common.idt>
	#include "OMX_Component.h"
#else
	//nothing
#endif

//common includes

#ifdef __MPEG4ENC_ARM_NMF //for ARM-NMF

	#define hcl_printf(x) 
	#define STORE_END_ALGO_TICKS(x) 
	#define PRINT_VAR_MY(x) 

	#undef METH
	#define METH(x) mpeg4enc_arm_nmf_parser::x

	#define COMP(x) mpeg4enc_arm_nmf_parser::x

	#define STATIC_FLAG

	#undef EXT_BIT
	#define EXT_BIT 0

#else //for MPC build

	#define COMP(x) x

	#define STATIC_FLAG static

	/*
	HACK:: ENS doesnt specify Buffer_t as shared mem
	and cause Buffer_t to be cached. Below typedef can
	cause memory penalties but no need to flush cache.
	*/
	typedef Buffer_t SHAREDMEM *Buffer_pOSI;
	#define Buffer_p Buffer_pOSI

#endif // for __MPEG4ENC_ARM_NMF


typedef struct
{
          t_uint16          ready;
          t_t1xhv_algo_id   algoId;
          t_uint32          addr_in_frame_buffer;
          t_uint32          addr_out_frame_buffer;
          t_uint32          addr_internal_buffer;
          t_uint32          addr_header_buffer;
          t_uint32          addr_in_bitstream_buffer;
          t_uint32          addr_out_bitstream_buffer;
          t_uint32          addr_in_parameters;
          t_uint32          addr_out_parameters;
          t_uint32          addr_in_frame_parameters;
          t_uint32          addr_out_frame_parameters;

} codec_param_type;

typedef struct
{
	ts_ddep_buffer_descriptor           bbm_desc;
	ts_ddep_buffer_descriptor           s_debug_buffer_desc;
} ts_ddep_vec_mpeg4_ddep_desc;


typedef struct {
ts_t1xhv_bitstream_buf_link			s_ddep_bitstream_buf_link; /* keep it first */
t_uint32							s_ddep_buffer_p;
#ifdef __MPEG4ENC_ARM_NMF
	t_uint16	reserved[16-(sizeof(ts_t1xhv_bitstream_buf_link)+sizeof(OMX_BUFFERHEADERTYPE))%16]; /*::CP t_uint16 -->> t_uint24 ???*/
#else
	t_uint16	reserved[16-(sizeof(ts_t1xhv_bitstream_buf_link)+sizeof(t_uint32))%16]; /*::CP t_uint16 -->> t_uint24 ???*/
#endif

} ts_ddep_bitstream_buf_link_and_header;


#define HEADER_BUFFER_SIZE 0x38

#define TOTAL_PORT_COUNT 2

// Define the clockSlot value for short header in 90Khz value
#define MP4_SH_CLOCK_SLOT           (3003)

// Define the rounding value for temporal reference computation
#define MP4_SH_ROUND_VALUE          (1500)

// Define supported size for MPEG4 short header mode. last two are custom mode
#define SVA_EC_MP4_SQCIF_WIDTH                              128
#define SVA_EC_MP4_SQCIF_HEIGHT                             96
#define SVA_EC_MP4_QCIF_WIDTH                               176
#define SVA_EC_MP4_QCIF_HEIGHT                              144
#define SVA_EC_MP4_CIF_WIDTH                                352
#define SVA_EC_MP4_CIF_HEIGHT                               288
#define SVA_EC_MP4_CIF4_WIDTH                               704
#define SVA_EC_MP4_CIF4_HEIGHT                              576
#define SVA_EC_MP4_CIF16_WIDTH                              1408
#define SVA_EC_MP4_CIF16_HEIGHT                             1152
#define SVA_EC_MP4_VGA_WIDTH                                640
#define SVA_EC_MP4_VGA_HEIGHT                               480
#define SVA_EC_MP4_QVGA_WIDTH                               320
#define SVA_EC_MP4_QVGA_HEIGHT                              240


// Define supported size for MPEG4 short header mode. last two are custom mode
#define SVA_EC_MP4_SH_SOURCE_FORMAT_SQCIF                   1
#define SVA_EC_MP4_SH_SOURCE_FORMAT_QCIF                    2
#define SVA_EC_MP4_SH_SOURCE_FORMAT_CIF                     3
#define SVA_EC_MP4_SH_SOURCE_FORMAT_CIF4                    4
#define SVA_EC_MP4_SH_SOURCE_FORMAT_CIF16                   5
#define SVA_EC_MP4_SH_SOURCE_FORMAT_VGA                     6
#define SVA_EC_MP4_SH_SOURCE_FORMAT_QVGA                    7
#define SVA_EC_MP4_SH_SOURCE_FORMAT_EXTENDED_PTYPE			7

// Define max header size in byte
#define SVA_EC_MP4_SH_MAX_HEADER_SIZE                       6

#define ONE_BIT 1
#define TWO_BITS 2
#define THREE_BITS 3
#define FOUR_BITS 4
#define EIGHT_BITS 8
#define NINE_BITS 9
#define ELEVEN_BITS 11
#define THIRTEEN_BITS 13
#define FIFTEEN_BITS 15
#define SIXTEEN_BITS 16
#define THIRTY_TWO_BITS 32


#define VOS_START_CODE 0x000001b0
#define VO_START_CODE 0x000001b5
#define VO_CODE 0x00000100
#define VOL_CODE 0x00000120
#define VOP_CODE 0x000001b6
#define VOS_END_CODE 0x000001b1

typedef struct
{
	t_uint48 buffer;
	t_uint32 nbBitsValid;
	t_uint16 *currBuffer;/*destination buffer*/
	t_uint16 *endbuffer;
	t_uint32 totalBitsWritten;/*total bits written*/
}t_sva_ec_writestream_header;

typedef enum
{
	SVA_COLOR_PRIMARY_UNKNOWN,
	SVA_COLOR_PRIMARY_BT601_FULLRANGE,
	SVA_COLOR_PRIMARY_BT601_LIMITEDRANGE,
	SVA_COLOR_PRIMARY_BT709_FULLRANGE,
	SVA_COLOR_PRIMARY_BT709_LIMITEDRANGE
} t_sva_ddep_color_primary;

#define SVA_RTYPE_MODE_CONSTANT_ZERO 0
#define SVA_RTYPE_MODE_CONSTANT_ONE 1

#define VFM_ROUND_UPPER(value, mask) (((t_uint32)(value) + mask ) & ~mask)
#define VFM_IMAGE_BUFFER_ALIGN_MASK               0xff
#define VFM_ADDRESS_ALIGN_MASK                    0xf

#ifndef SWAP16
	#define SWAP16(a) (((a&0xFF)<<8)|((a&0xFF00)>>8))
#endif

#endif // __MPEG4ENC_ARM_MPC_H

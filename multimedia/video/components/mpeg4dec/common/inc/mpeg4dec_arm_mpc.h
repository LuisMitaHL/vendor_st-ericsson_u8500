/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __MPEG4DEC_ARM_MPC_H
#define __MPEG4DEC_ARM_MPC_H


#include <stdio.h>


#ifdef __MPEG4DEC_ARM_NMF //for ARM-NMF
#include <string.h>
#include "Component.h"
#include "ENS_List.h"
#include <t1xhv_common.idt>
#include <vfm_common.idt>
#include "OMX_Component.h"
#else

#endif

//common includes

#ifdef __MPEG4DEC_ARM_NMF //for ARM-NMF

	#define START_CODE_VALUE_SH 0x00008002ul
    #define START_CODE_VALUE_SP 0x000001B6ul
    #define MULTIPLYING_FACTOR 1
	#define LENGTH nFilledLen
	#define RETURNBUFFER(x,y) ReleaseBuffer(x,y)
	#define ADDRESS pBuffer
	#define AHB_	(t_ahb_address)
	#define FLAGS nFlags
	#define ALLOCLENGTH nAllocLen
	#define ENDALGO  
    #define convfrom16bitmode(x) x
	#define convto16bitmode(x) x
	#define SVP_SHAREDMEM_FROM16(x) x
	#define SVP_SHAREDMEM_TO16(x) x
	#define SVP_SHAREDMEM_FROM24(x) x
	#define SVP_SHAREDMEM_TO24(x) x
	#undef METH
	#define METH(x) mpeg4dec_arm_nmf_ddep::x
	#define COMP(x) mpeg4dec_arm_nmf_ddep::x
	#define STATIC_FLAG
	#undef SHAREDMEM
	#define SHAREDMEM
	#define PUT_PRAGMA
	#define PRINT_VAR(x)

	#undef EXT_BIT
	#define EXT_BIT 0

	#undef Buffer_p
	#define Buffer_p (OMX_BUFFERHEADERTYPE_p)
	#define Buffer_t OMX_BUFFERHEADERTYPE
#else //for MPC build
	#define START_CODE_VALUE_SH 0x00000280ul
    #define START_CODE_VALUE_SP 0x0000B601ul
	#define MULTIPLYING_FACTOR 2
	#define LENGTH filledLen
	#define RETURNBUFFER(x,y) Port_returnBuffer(&mPorts[x],y)
	#define ADDRESS address
	#define AHB_	
	#define FLAGS flags
	#define ALLOCLENGTH allocLen
	#define ENDALGO &iEndAlgo,
	#define EXT_BIT 1
	//#define METHOD(x) METH(x)
    #define COMP(x) x
	#define STATIC_FLAG static
	#define PUT_PRAGMA #pragma force_dcumode

	#define OMX_TRUE 	1
	#define OMX_FALSE 	0
	typedef Buffer_t SHAREDMEM *Buffer_pOSI;
	#define Buffer_p Buffer_pOSI

	#if !(defined(__svp8500_ed__) || defined(__svp8500_v1__))
		#define SVP_SHAREDMEM_FROM16(x) convfrom16bitmode(x)
		#define SVP_SHAREDMEM_TO16(x) convto16bitmode(x)
		#define SVP_SHAREDMEM_FROM24(x) x
		#define SVP_SHAREDMEM_TO24(x) x
	//#error "mop selected"
	#else
		#define SVP_SHAREDMEM_FROM16(x) ((((x)>>16)&0xFFFFul)|(((x)<<16)&0xFFFF0000ul))
		#define SVP_SHAREDMEM_TO16(x) ((((x)>>16)&0xFFFFul)|(((x)<<16)&0xFFFF0000ul))
		#define SVP_SHAREDMEM_FROM24(x) ((((x)>>24)&0xFFFFFFul)|(((x)<<24)&0xFFFFFF000000ul))
		#define SVP_SHAREDMEM_TO24(x) ((((x)>>24)&0xFFFFFFul)|(((x)<<24)&0xFFFFFF000000ul))
	#endif	//for !(defined(__svp8500_ed__) || defined(__svp8500_v1__))
#endif // for __JPEGDENC_ARM_NMF


#define TOTAL_PORT_COUNT 	3
#define NO_OF_HEADERS		32
#define INPUT_BUFFER_COUNT 	32
#define OUTPUT_BUFFER_COUNT 32
#define EMPTY_BUFFER 2 //  it means empty buffer was provided at input
#define HANDLE_EOS   1
#define MINIMUM(X, Y)  ((X) < (Y) ? (X) : (Y))
typedef struct
{
          t_uint16          ready;
          t_t1xhv_algo_id   algoId;
          t_uint32          addr_in_frame_buffer;
          t_uint32          addr_out_frame_buffer;
          t_uint32          addr_internal_buffer;
          t_uint32          addr_in_bitstream_buffer;
          t_uint32          addr_out_bitstream_buffer;
          t_uint32          addr_in_parameters;
          t_uint32          addr_out_parameters;
          t_uint32          addr_in_frame_parameters;
          t_uint32          addr_out_frame_parameters;

}
codec_param_type;


typedef struct
{
	ts_ddep_buffer_descriptor			bbm_desc;
	ts_ddep_buffer_descriptor			s_debug_buffer_desc;
} ts_ddep_vdc_mpeg4_ddep_desc;

typedef struct {
ts_t1xhv_bitstream_buf_link			s_ddep_bitstream_buf_link; /* keep it first */
t_uint32							s_ddep_buffer_p; //-FIXME -AmitC
t_uint16							reserved[16-(sizeof(ts_t1xhv_bitstream_buf_link)+sizeof(t_uint32))%16]; /*::CP t_uint16 -->> t_uint24 ???*/
} ts_ddep_bitstream_buf_link_and_header;


typedef struct
{
	t_uint16 index_to_next_not_required_bitstream_buffer;
	t_uint16 current_bitstream_buffer_index;
	t_uint16 last_index_ps_ddep_bitstream_buf_link_and_header;
	t_ahb_address physical_ps_ddep_bitstream_buf_link_and_header;
	t_uint32 ps_ddep_bitstream_buf_link_and_header;   //this  is redundant
} t_ddep_bitstream_mgmt;

#endif // __JPEGENC_ARM_MPC_H

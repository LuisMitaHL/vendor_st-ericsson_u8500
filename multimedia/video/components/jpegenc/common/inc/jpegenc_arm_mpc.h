/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


#ifndef __JPEGENC_ARM_MPC_H
#define __JPEGENC_ARM_MPC_H



#include <stdio.h>


#ifdef __JPEGDENC_ARM_NMF //for ARM-NMF
#include <string.h>
//#include <jpegenc/arm_nmf/parser.nmf>
#include "Component.h"
#include "ENS_List.h"
#include <t1xhv_common.idt>
#include <t1xhv_vec.idt>
#include <vfm_common.idt>
//#include <arm_nmf/share/vfm_vec_jpeg.idt>
#include "OMX_Component.h"
#else

#endif

//common includes

#ifdef __JPEGDENC_ARM_NMF //for ARM-NMF


    #define convfrom16bitmode(x) x
	#define SVP_SHAREDMEM_FROM16(x) x
	#define SVP_SHAREDMEM_TO16(x) x
	#define SVP_SHAREDMEM_FROM24(x) x
	#define SVP_SHAREDMEM_TO24(x) x

	#undef METH
	#define METH(x) jpegenc_arm_nmf_parser::x
	#define COMP(x) jpegenc_arm_nmf_parser::x
	#define STATIC_FLAG
	#undef SHAREDMEM
	#define SHAREDMEM
	#define PUT_PRAGMA
	#define PRINT_VAR(x)

	#define ADD_40  0x0
	#define ADD_100 0x0
	#define ADD_80	0x0

	#undef EXT_BIT
	#define EXT_BIT 0

#else //for MPC build
	#define ADD_80 0x80
	#define ADD_100 0x100
	#define ADD_40 0x40
	#define COMP(x) x
	#define STATIC_FLAG static
	#define PUT_PRAGMA #pragma force_dcumode

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


#define NB_PORTS 2



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

}
codec_param_type;

typedef enum
{
	DDEP_JPEGE_SLICE_TYPE_LAST = 0,
	DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST,
	DDEP_JPEGE_SLICE_TYPE_FIRST,
	DDEP_JPEGE_SLICE_TYPE_SUBSEQUENT,
	DDEP_JPEGE_SLICE_TYPE_FIRST_SKIPPED
} t_ddep_jpege_slice_type;




typedef struct
{
	ts_ddep_buffer_descriptor			bbm_desc;
	ts_ddep_buffer_descriptor			s_debug_buffer_desc;
} ts_ddep_sec_jpeg_ddep_desc;


typedef struct {
ts_t1xhv_bitstream_buf_link			s_ddep_bitstream_buf_link; /* keep it first */
t_uint32							s_ddep_buffer_p;
#ifdef __JPEGDENC_ARM_NMF
	t_uint16	reserved[16-(sizeof(ts_t1xhv_bitstream_buf_link)+sizeof(OMX_BUFFERHEADERTYPE))%16]; /*::CP t_uint16 -->> t_uint24 ???*/
#else
	t_uint16	reserved[16-(sizeof(ts_t1xhv_bitstream_buf_link)+sizeof(Buffer_t))%16]; /*::CP t_uint16 -->> t_uint24 ???*/
#endif

} ts_ddep_bitstream_buf_link_and_header;


//static void common_ReleaseBuffer(void *,t_uint32 port_idx,Buffer_p buf);
//static void common_release_resources(void *);
//static t_uint16 common_buffer_available_atinput(void *);
//static t_uint16 common_buffer_available_atoutput(void *);
//static void common_linkin_loop(void *);
//static void common_update_link(void *);
//static void common_create_link(void *);
//static void common_program_link(void *);
//static void common_setNeeds(void *);
//static void common_download_parameters(void *);
//static void common_setParameter(void *);





#endif // __JPEGENC_ARM_MPC_H

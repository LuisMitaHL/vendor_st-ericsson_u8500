/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef PACKET_VIDEO_SUPPORT
#define LOG_TAG "H264HAMAC"
#include<cutils/log.h>
#endif

#define PRINTLOG

#include "settings.h"
#include "types.h"
#include "host_types.h"
#include "hamac_types.h"
#include "host_decoder.h"
#include "local_alloc.h"

#ifdef MALLOC_DEBUG
#define GetPhysicallAddr(a)		(printf("Get Physicall at %s %d %p\n",__FILE__,__LINE__,a),VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)a))
#define GetLogicalAddr(a)		(printf("Get Logicall at %s %d %p\n",__FILE__,__LINE__,a),VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)a))
#else
#define GetPhysicallAddr(a)		VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)a)
#define GetLogicalAddr(a)		VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)a)
#endif

#ifdef NO_HAMAC
#define CONVERT_TO_AHB_ADDRESS_BE(a)   (t_address) (a)
#define CONVERT_TO_MTF_AHB_ADDRESS(a)   (t_address) (a)
#define UNCONVERT_TO_MTF_AHB_ADDRESS(a)   (t_address) (a)
#define MTF_MARKER(a) a
#define UNMARK_MTF(a) a
#else
#define CONVERT_TO_AHB_ADDRESS_BE(a)   		(t_address) Endianess(GetPhysicallAddr(a))
#define CONVERT_TO_MTF_AHB_ADDRESS(a)   	(t_address) Endianess(MTF_MARKER(GetPhysicallAddr(a)))
#define UNCONVERT_TO_MTF_AHB_ADDRESS(a)   	(t_address) GetLogicalAddr( UNMARK_MTF(Endianess((t_uint32) (a))))
#define MTF_MARKER(a)	((t_uint32) (a)	| 1 )
#define UNMARK_MTF(a)	((t_uint32) (a)	&  0xFFFFFFFE )
#endif

void HamacFillFrameBuffIn(void* local_vfmmemory_ctxt, ts_t1xhv_vec_frame_buf_in* p, t_host_info* p_host)
{
    p->addr_source_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_source_buffer);
    p->addr_fwd_ref_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_ref_frame);
    p->addr_grab_ref_buffer = 0;
    p->addr_intra_refresh_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_intra_refresh_buffer);
}

void HamacFillFrameBuffOut(void* local_vfmmemory_ctxt, ts_t1xhv_vec_frame_buf_out* p, t_host_info* p_host)
{
    p->addr_dest_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_dest_buffer);
    p->addr_deblocking_param_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->p_deblocking_paramv);
    p->addr_motion_vector_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_motion_vector_buffer);
    p->addr_intra_refresh_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_intra_refresh_buffer);
}

void HamacFillInternals(void* local_vfmmemory_ctxt, ts_t1xhv_vec_internal_buf* p, t_host_info* p_host)
{
//#if (defined  __ndk5500_a0__) // && (defined TMP_ALLOC_ESRAM))
#ifdef __ndk5500_a0__
    p->addr_search_window_buffer = (t_ahb_address)p_host->buff->addr_search_window_buffer;
    p->addr_search_window_end = (t_address)(p_host->buff->addr_search_window_buffer) + p_host->buff->sw_size;
#else
    p->addr_search_window_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_search_window_buffer);
    p->addr_search_window_end = (t_address)Endianess(GetPhysicallAddr(p_host->buff->addr_search_window_buffer)+p_host->buff->sw_size);
#endif
    p->addr_jpeg_run_level_buffer = 0;
    p->addr_h264e_H4D_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_H4D_buffer);
#ifndef __ndk5500_a0__
    if (!p_host->enable_hdtv)
        p->addr_h264e_rec_local = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_rec_local);
    p->addr_h264e_cup_context = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_cup_context);
#endif
#ifdef  __ndk5500_a0__
#ifdef TMP_ALLOC_ESRAM
    p->addr_h264e_rec_local = (t_ahb_address)p_host->buff->addr_h264e_rec_local;
//    p->addr_h264e_cup_context = (t_ahb_address)p_host->buff->addr_h264e_cup_context;
    p->addr_h264e_cup_context = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_cup_context);
#else
    p->addr_h264e_rec_local = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_rec_local);
    p->addr_h264e_cup_context = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_cup_context);
#endif
#endif
}

void HamacFillBistreamOut(void* local_vfmmemory_ctxt, ts_t1xhv_bitstream_buf_pos* p, ts_t1xhv_bitstream_buf* t, t_host_info* p_host)
{
    t_uint32 start_addr, aligned_addr;

    printf("addr_bitstream_buffer(logical address): 0x%x\n",(unsigned) p_host->buff->addr_bitstream_buffer);
    start_addr = (t_uint32) GetPhysicallAddr(p_host->buff->addr_bitstream_buffer);
    aligned_addr = start_addr & 0xFFFFFFF0;

    p->addr_bitstream_buf_struct = Endianess(MTF_MARKER(GetPhysicallAddr(t)));
    p->addr_bitstream_start = Endianess(aligned_addr);
    p->bitstream_offset = Endianess((start_addr - aligned_addr) * 8);
    p->reserved_1 = 0;

    t->addr_buffer_start = Endianess(aligned_addr);
    t->addr_buffer_end = Endianess(aligned_addr + p_host->buff->bs_size);
    t->addr_window_start = Endianess(aligned_addr);
    t->addr_window_end = Endianess(aligned_addr + p_host->buff->bs_size);
}

void HamacFillParameterIn(void *local_vfmmemory_ctxt, ts_t1xhv_vec_h264_param_in *p, t_host_info *p_host)
{
#ifdef __ndk5500_a0__
    t_uint32 start_addr, aligned_addr;
    start_addr = (t_uint32) GetPhysicallAddr(p_host->buff->addr_bitstream_buffer);
    aligned_addr = start_addr & 0xFFFFFFF0;

#if 0
    p-> parsing_error = 0;
    p->DBLK_flag = p_host->buff->DBLK_mode;
    p->ERC_used = 1;
    p->intra_conc = p_host->buff->intra_conc;
    p->reserved_2 = 0;
#endif
    p->addr_source_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_source_buffer);
    p->addr_fwd_ref_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_ref_frame);
    /* TODO FB V2 only!
       p->addr_intra_refresh_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_intra_refresh_buffer);
     */

    p->addr_rec_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_dest_buffer);
    p->addr_output_bitstream_start = Endianess(aligned_addr);
    p->addr_output_bitstream_end = Endianess(aligned_addr + p_host->buff->bs_size);
    p->bitstream_offset = Endianess((start_addr - aligned_addr) * 8);
    p->addr_sequence_info_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_sequence_info_buffer);

#ifdef TMP_ALLOC_ESRAM
//    p->addr_ectx = (t_ulong_value)p_host->buff->addr_h264e_cup_context;
    p->addr_ectx = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_cup_context);
    p->addr_external_sw = (t_ulong_value)p_host->buff->addr_search_window_buffer;
    p->addr_local_rec_buffer = (t_ulong_value)p_host->buff->addr_h264e_rec_local;
//    p->addr_external_cwi = CONVERT_TO_MTF_AHB_ADDRESS(p_host->buff->addr_external_cwi);
    p->addr_external_cwi = (t_ulong_value)p_host->buff->addr_external_cwi;
//    p->addr_sequence_info_buffer = (t_ulong_value)p_host->buff->addr_sequence_info_buffer;
#else
    p->addr_ectx = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_cup_context);
    p->addr_external_sw = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_search_window_buffer);
    p->addr_local_rec_buffer = CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->addr_h264e_rec_local);
//    p->addr_external_cwi = CONVERT_TO_MTF_AHB_ADDRESS(p_host->buff->addr_external_cwi);
    p->addr_external_cwi = CONVERT_TO_MTF_AHB_ADDRESS(p_host->buff->addr_external_cwi);
   // p->addr_sequence_info_buffer = CONVERT_TO_MTF_AHB_ADDRESS(p_host->buff->addr_sequence_info_buffer);
#endif
#endif
}

#define HAMACALLOC(a,n)  (a*) mallocCM((sizeof(a)*n), 8-1)

void HamacAllocParameter(void* vfm_memory_ctxt, t_hamac_param* t)
{
	PRINTLOG("> In HamacAllocParameter ");
    /* MTF */
    t->addr_in_frame_buffer      = HAMACALLOC(ts_t1xhv_vec_frame_buf_in, CMD_FIFO_SIZE);
    PRINTLOG("Allocated t->addr_in_frame_buffer : 0x%x ",(t_uint32)t->addr_in_frame_buffer);
    t->addr_out_frame_buffer     = HAMACALLOC(ts_t1xhv_vec_frame_buf_out, CMD_FIFO_SIZE);
    PRINTLOG("Allocated t->addr_out_frame_buffer : 0x%x ",(t_uint32)t->addr_out_frame_buffer);
    t->addr_internal_buffer      = HAMACALLOC(ts_t1xhv_vec_internal_buf, 1);
    PRINTLOG("Allocated t->addr_internal_buffer : 0x%x ",(t_uint32)t->addr_internal_buffer);
    t->addr_in_bitstream_buffer  = HAMACALLOC(ts_t1xhv_bitstream_buf_pos, CMD_FIFO_SIZE);
    PRINTLOG("Allocated t->addr_in_bitstream_buffer : 0x%x ",(t_uint32)t->addr_in_bitstream_buffer);
    t->addr_out_bitstream_buffer = HAMACALLOC(ts_t1xhv_bitstream_buf_pos, 1);
    PRINTLOG("Allocated t->addr_out_bitstream_buffer : 0x%x ",(t_uint32)t->addr_out_bitstream_buffer);
    t->addr_in_parameters        = HAMACALLOC(ts_t1xhv_vec_h264_param_in, CMD_FIFO_SIZE);
    PRINTLOG("Allocated t->addr_in_parameters : 0x%x ",(t_uint32)t->addr_in_parameters);
    t->addr_out_parameters       = HAMACALLOC(ts_t1xhv_vec_h264_param_out, CMD_FIFO_SIZE);
    PRINTLOG("Allocated t->addr_out_parameters : 0x%x ",(t_uint32)t->addr_out_parameters);
    t->addr_in_frame_parameters  = HAMACALLOC(ts_t1xhv_vec_h264_param_inout, CMD_FIFO_SIZE);
    PRINTLOG("Allocated t->addr_in_frame_parameters : 0x%x ",(t_uint32)t->addr_in_frame_parameters);
    t->addr_out_frame_parameters = HAMACALLOC(ts_t1xhv_vec_h264_param_inout, 1);
    PRINTLOG("Allocated t->addr_out_frame_parameters : 0x%x ",(t_uint32)t->addr_out_frame_parameters);
    t->addr_bitstream_buf_struct = HAMACALLOC(ts_t1xhv_bitstream_buf, CMD_FIFO_SIZE);
    PRINTLOG("Allocated t->addr_bitstream_buf_struct : 0x%x ",(t_uint32)t->addr_bitstream_buf_struct);
#ifdef __ndk5500_a0__
  //  t->addr_in_parameters->addr_param_inout = (t_ulong_value)mallocCM(sizeof(ts_t1xhv_vec_h264_param_inout), 0x1);
  /*Conversion is done here*/
   t->addr_in_parameters->addr_param_inout = (t_ulong_value)VFM_GetPhysical(vfm_memory_ctxt, (t_uint8 *)t->addr_out_frame_parameters);
   PRINTLOG("Allocated t->addr_in_parameters->addr_param_inout : 0x%x ",(t_uint32)t->addr_in_parameters->addr_param_inout);
   //t->addr_in_parameters->addr_param_inout = (t_ulong_value)VFM_GetPhysical(vfm_memory_ctxt, (t_uint8 *)t->addr_in_parameters->addr_param_inout );
//  t->addr_in_parameters->addr_param_inout = (t_ulong_value)t->addr_out_frame_parameters;
#endif
	PRINTLOG("< In HamacAllocParameter ");
}

void HamacDeallocParameter(void* vfm_memory_ctxt, t_hamac_param* t)
{
	PRINTLOG("> In HamacDeallocParameter ");
	PRINTLOG("Abt to free t->addr_in_frame_buffer : 0x%x ",(t_uint32)t->addr_in_frame_buffer);
	if (t->addr_in_frame_buffer)
	{
    	free(t->addr_in_frame_buffer);
    	t->addr_in_frame_buffer = 0x0;
	}
    PRINTLOG("Abt to free t->addr_out_frame_buffer : 0x%x ",(t_uint32)t->addr_out_frame_buffer);
    if (t->addr_out_frame_buffer)
    {
    	free(t->addr_out_frame_buffer);
    	t->addr_out_frame_buffer = 0x0;
	}
    PRINTLOG("Abt to free t->addr_internal_buffer : 0x%x ",(t_uint32)t->addr_internal_buffer);
    if (t->addr_internal_buffer)
    {
    	free(t->addr_internal_buffer);
    	t->addr_internal_buffer = 0x0;
	}
    PRINTLOG("Abt to free t->addr_in_bitstream_buffer : 0x%x ",(t_uint32)t->addr_in_bitstream_buffer);
    if (t->addr_in_bitstream_buffer)
    {
    	free(t->addr_in_bitstream_buffer);
    	t->addr_in_bitstream_buffer = 0x0;
	}
    PRINTLOG("Abt to free t->addr_out_bitstream_buffer : 0x%x ",(t_uint32)t->addr_out_bitstream_buffer);
    if (t->addr_out_bitstream_buffer)
    {
    	free(t->addr_out_bitstream_buffer);
    	t->addr_out_bitstream_buffer =0x0;
	}
#ifdef __ndk5500_a0__
    //free(t->addr_in_parameters->addr_param_inout);
#endif
	PRINTLOG("Abt to free t->addr_in_parameters : 0x%x ",(t_uint32)t->addr_in_parameters);
	if (t->addr_in_parameters)
	{
    	free(t->addr_in_parameters);
    	t->addr_in_parameters = 0x0;
	}
    PRINTLOG("Abt to free t->addr_out_parameters : 0x%x ",(t_uint32)t->addr_out_parameters);
    if (t->addr_out_parameters)
    {
    	free(t->addr_out_parameters);
    	t->addr_out_parameters =0x0;
	}
    PRINTLOG("Abt to free t->addr_in_frame_parameters : 0x%x ",(t_uint32)t->addr_in_frame_parameters);
    if (t->addr_in_frame_parameters)
    {
    	free(t->addr_in_frame_parameters);
    	t->addr_in_frame_parameters = 0x0;
	}
    PRINTLOG("Abt to free t->addr_out_frame_parameters : 0x%x ",(t_uint32)t->addr_out_frame_parameters);
    if (t->addr_out_frame_parameters)
    {
    	free(t->addr_out_frame_parameters);
    	t->addr_out_frame_parameters =0x0;
	}
    PRINTLOG("Abt to free t->addr_bitstream_buf_struct : 0x%x ",(t_uint32)t->addr_bitstream_buf_struct);
    if (t->addr_bitstream_buf_struct)
    {
    	free(t->addr_bitstream_buf_struct);
    	t->addr_bitstream_buf_struct = 0x0;
	}
    PRINTLOG("< In HamacDeallocParameter ");
}

#if VERBOSE_PARAM

void DisplayFrameBuffIn(ts_t1xhv_vec_frame_buf_in *p)
{
    printf("HOST: addr_source_buffer        : %lu\n", p->addr_source_buffer);
    printf("HOST: addr_fwd_ref_buffer       : %lu\n", p->addr_fwd_ref_buffer);
    printf("HOST: addr_grab_ref_buffer      : %lu\n", p->addr_grab_ref_buffer);
    printf("HOST: addr_intra_refresh_buffer : %lu\n", p->addr_intra_refresh_buffer);
}

void DisplayFrameBuffOut(ts_t1xhv_vec_frame_buf_out *p)
{
    printf("HOST: addr_dest_buffer             : %lu\n", p->addr_dest_buffer);
    printf("HOST: addr_deblocking_param_buffer : %lu\n", p->addr_deblocking_param_buffer);
    printf("HOST: addr_motion_vector_buffer    : %lu\n", p->addr_motion_vector_buffer);
    printf("HOST: addr_intra_refresh_buffer    : %lu\n", p->addr_intra_refresh_buffer);
}

void DisplayInternalBuff(ts_t1xhv_vec_internal_buf  *p)
{
    printf("HOST: addr_search_window_buffer  : %lu\n", p->addr_search_window_buffer);
    printf("HOST: addr_search_window_end     : %lu\n", p->addr_search_window_end);
    printf("HOST: addr_jpeg_run_level_buffer : %lu\n", p->addr_jpeg_run_level_buffer);
    printf("HOST: addr_h264e_H4D_buffer      : %lu\n", p->addr_h264e_H4D_buffer);
}

void DisplayParamIn(ts_t1xhv_vec_h264_param_in *p)
{
#ifdef __ndk5500_a0__
    printf("HOST: param_in addr_source_buffer           :  %p\n", p->addr_source_buffer);
    printf("HOST: param_in addr_fwd_ref_buffer          :  %p\n", p->addr_fwd_ref_buffer);
    printf("HOST: param_in addr_rec_buffer              :  %p\n", p->addr_rec_buffer);
    printf("HOST: param_in addr_output_bitstream_start  :  %p\n", p->addr_output_bitstream_start);
    printf("HOST: param_in addr_output_bitstream_end    :  %p\n", p->addr_output_bitstream_end);
    printf("HOST: param_in bitstream_offset             :  %ld\n", p->bitstream_offset);

    printf("HOST: param_in addr_ectx                    :  %p\n", p->addr_ectx);
    printf("HOST: param_in addr_external_sw             :  %p\n", p->addr_external_sw);
    printf("HOST: param_in addr_local_rec_buffer        :  %p\n", p->addr_local_rec_buffer);
    printf("HOST: param_in addr_external_cwi            :  %p\n", p->addr_external_cwi);
    printf("HOST: param_in addr_sequence_info_buffer    :  %p\n", p->addr_sequence_info_buffer);
    printf("HOST: param_in addr_param_inout             :  %p\n", p->addr_param_inout);
#endif
}

void DisplayParam(t_hamac_param	*t)
{
    DisplayFrameBuffIn(t->addr_in_frame_buffer);
    DisplayFrameBuffOut(t->addr_out_frame_buffer);
    DisplayInternalBuff(t->addr_internal_buffer);
    DisplayParamIn(t->addr_in_parameters);
}
#endif

void HamacFillParamIn(void *local_vfmmemory_ctxt, t_dec *dec, t_uint16 nslices, t_hamac_param *t)
{
    HamacFillFrameBuffIn(local_vfmmemory_ctxt, t->addr_in_frame_buffer, &dec->host);
    HamacFillFrameBuffOut(local_vfmmemory_ctxt, t->addr_out_frame_buffer, &dec->host);
    HamacFillInternals(local_vfmmemory_ctxt, t->addr_internal_buffer, &dec->host);
    HamacFillBistreamOut(local_vfmmemory_ctxt, t->addr_in_bitstream_buffer, t->addr_bitstream_buf_struct, &dec->host);
    HamacFillParameterIn(local_vfmmemory_ctxt, t->addr_in_parameters, &dec->host);

#if VERBOSE_PARAM
    DisplayParam(t);
#endif
}

#define HamacToPhysical(address,type) do { address = (type) MTF_MARKER(GetPhysicallAddr(address)); } while (0)

void HamacToPhysicalAddresses(void* local_vfmmemory_ctxt, t_hamac_param* t)
{
    /* NO Endianess is needed for pointers above , because NMF marshalling take care of this conversion. */
	HamacToPhysical(t->addr_in_frame_buffer,      ts_t1xhv_vec_frame_buf_in*);
	HamacToPhysical(t->addr_out_frame_buffer,     ts_t1xhv_vec_frame_buf_out*);
	HamacToPhysical(t->addr_internal_buffer,      ts_t1xhv_vec_internal_buf*);
	HamacToPhysical(t->addr_in_bitstream_buffer,  ts_t1xhv_bitstream_buf_pos*);
	HamacToPhysical(t->addr_out_bitstream_buffer, ts_t1xhv_bitstream_buf_pos*);
	HamacToPhysical(t->addr_in_parameters,        ts_t1xhv_vec_h264_param_in*);
	HamacToPhysical(t->addr_out_parameters,       ts_t1xhv_vec_h264_param_out*);
	HamacToPhysical(t->addr_in_frame_parameters,  ts_t1xhv_vec_h264_param_inout*);
	HamacToPhysical(t->addr_out_frame_parameters, ts_t1xhv_vec_h264_param_inout*);
#ifdef __ndk5500_a0__
    //t->addr_in_parameters->addr_param_inout = (t_ulong_value)t->addr_out_frame_parameters;
   // t->addr_in_parameters->addr_param_inout = (t_ulong_value)VFM_GetPhysical(vfm_memory_ctxt, (t_uint8 *)t->addr_in_parameters->addr_param_inout);
   // (t_ulong_value)VFM_GetPhysical(vfm_memory_ctxt, (t_uint8 *)t->addr_in_parameters->addr_param_inout );
  //  t->addr_in_parameters->addr_param_inout = (t_ulong_value)GetPhysicallAddr(t->addr_in_parameters->addr_param_inout);
#endif
}

#define HamacToLogical(address,type) do { address = (type) VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(address)); } while (0)

void HamacToLogicalAddresses(void* local_vfmmemory_ctxt, t_hamac_param* t)
{
    HamacToLogical(t->addr_in_frame_buffer,      ts_t1xhv_vec_frame_buf_in*);
    HamacToLogical(t->addr_out_frame_buffer,     ts_t1xhv_vec_frame_buf_out*);
    HamacToLogical(t->addr_internal_buffer,      ts_t1xhv_vec_internal_buf*);
    HamacToLogical(t->addr_in_bitstream_buffer,  ts_t1xhv_bitstream_buf_pos*);
    HamacToLogical(t->addr_out_bitstream_buffer, ts_t1xhv_bitstream_buf_pos*);
    HamacToLogical(t->addr_in_parameters,        ts_t1xhv_vec_h264_param_in*);
    HamacToLogical(t->addr_out_parameters,       ts_t1xhv_vec_h264_param_out*);
    HamacToLogical(t->addr_in_frame_parameters,  ts_t1xhv_vec_h264_param_inout*);
    HamacToLogical(t->addr_out_frame_parameters, ts_t1xhv_vec_h264_param_inout*);
#ifdef __ndk5500_a0__
  //  t->addr_in_parameters->addr_param_inout = (t_ulong_value)t->addr_out_frame_parameters; /*not necessary!?*/
   //t->addr_in_parameters->addr_param_inout = (t_ulong_value)VFM_GetLogical(vfm_memory_ctxt, (t_uint8 *)t->addr_in_parameters->addr_param_inout);
#endif
}


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "types.h"

#ifdef NMF_BUILD
#include <h264enc/arm_nmf/encoder.nmf>
#endif

#include "settings.h"
#include "types.h"
#include "hamac_types.h"
#include "interface.h"
#include "common_bitstream.h"
#include "host_bitstream.h"
//#include "host_sei.h"
#include "host_block_info.h"
#include "hamac_bitstream.h"
#include "hamac_deblocking.h"
#include "host_mem.h"
#include "host_decoder.h"
#include "host_hamac_pipe.h"
#include "user_interface.h"
#include "local_alloc.h"
#include <omx_define.h>
#include "h264enc_host_parset.h"
#ifdef __ndk5500_a0__
#include "hva_cmds.h"
#endif
#include "h264enc_host_sei.h"
#include "VFM_Memory.h"

    #include "OstTraceDefinitions_ste.h"
    #include "OpenSystemTrace_ste.h"
    #ifdef OST_TRACE_COMPILER_IN_USE
        #include "video_components_h264enc_arm_nmf_h264enc_encoder_src_host_encoderTraces.h"
    #endif


#define MAX_SEI_SIZE 256

/*def in hamac_types.h*/
//#define Endianess(a)	(((((t_uint32)(a)) & 0xffff )<<16) + ((((t_uint32)(a)) >> 16) & 0xffff ))

extern "C" t_BRC_SEI* SEIio;

typedef enum {
	I_SLICE,
	P_SLICE
} te_fhe_slice_type;

/**
 * \brief Decoder main function.
 */

#define SVA_MM_ALIGN_WORD       0x1
#define SVA_MM_ALIGN_256BYTES   0xff

#ifdef MALLOC_DEBUG
#define GetPhysicallAddr(a)		(printf("Get Physicall at %s %d %p\n",__FILE__,__LINE__,a),VFM_GetPhysical(vfm_memory_ctxt, (t_uint8 *)a))
#define GetLogicalAddr(a)		(printf("Get Logicall at %s %d %p\n",__FILE__,__LINE__,a),VFM_GetLogical(vfm_memory_ctxt, (t_uint8 *)a))
#else
#define GetPhysicallAddr(a)		VFM_GetPhysical(vfm_memory_ctxt, (t_uint8 *)a)
#define GetLogicalAddr(a)		VFM_GetLogical(vfm_memory_ctxt, (t_uint8 *)a)
#endif

extern "C" void * my_vfm_memory_ctxt;

void memcpy_(void* dst, void* src, int size)
{
	int j;
	for(j = 0; j < size; j++)
	{
		((t_uint8*)dst)[j] = ((t_uint8*)src)[j];
	}
}

void memset_(void* dst, t_uint8 val, int size)
{
	int j;
	for(j = 0; j < size; j++)
	{
		((t_uint8*)dst)[j] = val;
	}
}

void METH(dump_struct)(const char* name, t_uint32 pic, void* ptr_, t_uint32 size)
{
		t_uint32 u;
		t_uint8* ptr = (t_uint8*)ptr_;

        OstTraceFiltInst2(TRACE_FLOW, "# Pic %d, Size %d", pic, size); // %s not supported
		for (u = 0; u < size; u += 4)
        {
            OstTraceFiltInst1(TRACE_FLOW, "%02x:", u);
            OstTraceFiltInst4(TRACE_FLOW, "%02x %02x %02x %02x", ptr[u+1], ptr[u+0], ptr[u+3], ptr[u+2]);
        }
        OstTraceFiltInst0(TRACE_FLOW, "");
}

void METH(dump_params)(t_hamac_param *h_param)
{

	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_in_frame_buffer 0x%x", (t_uint32)h_param->addr_in_frame_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_out_frame_buffer 0x%x", (t_uint32)h_param->addr_out_frame_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_internal_buffer 0x%x", (t_uint32)h_param->addr_internal_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_in_bitstream_buffer 0x%x", (t_uint32)h_param->addr_in_bitstream_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_out_bitstream_buffer 0x%x", (t_uint32)h_param->addr_out_bitstream_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_in_parameters 0x%x", (t_uint32)h_param->addr_in_parameters);
	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_out_parameters 0x%x", (t_uint32)h_param->addr_out_parameters);
	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_in_frame_parameters 0x%x", (t_uint32)h_param->addr_in_frame_parameters);
	OstTraceFiltInst1(TRACE_FLOW, "> h_param->addr_out_frame_parameters 0x%x", (t_uint32)h_param->addr_out_frame_parameters);
}

void METH(call_dsp)(void *mem_ctxt, t_hamac_param *h_param)
{
    OstTraceFiltInst0(TRACE_FLOW, "Call DSP");
	//dump_params(&hamac_param);
	//dump_params(h_param);
	OstTraceFiltInst0(TRACE_FLOW, "H264Enc ARM NMF: About to call HamacToPhysicalAddresses");
	HamacToPhysicalAddresses(vfm_memory_ctxt, h_param);
	dump_params(h_param);
#if 1 /* TEST_PERFS */
#ifdef __ndk5500_a0__
	NMF_LOG("Start Encode Task : task_desc:0x%lx HVA profile:%d\n", (t_uint32)h_param->addr_in_parameters, H264_ENC);
	iStartCodec.startTask((t_uint32)h_param->addr_in_parameters, (t_uint32)this, H264_ENC );
#else
	iStartCodec.startCodec(
			(t_uint32)h_param->addr_in_frame_buffer,
			(t_uint32)h_param->addr_out_frame_buffer,
			(t_uint32)h_param->addr_internal_buffer,
			0, /* addr_in_header_buffer */
			(t_uint32)h_param->addr_in_bitstream_buffer,
			(t_uint32)h_param->addr_out_bitstream_buffer,
			(t_uint32)h_param->addr_in_parameters,
			(t_uint32)h_param->addr_out_parameters,
			(t_uint32)h_param->addr_in_frame_parameters,
			(t_uint32)h_param->addr_out_frame_parameters);
#endif
#endif
    OstTraceFiltInst0(TRACE_FLOW, "DSP called");
	HamacToLogicalAddresses(vfm_memory_ctxt, h_param);
	dump_params(h_param);
#if 0 /* TEST_PERFS */
	endCodec(STATUS_JOB_COMPLETE, VEC_ERT_NONE, 0);
#endif
}

void* METH(AllocESRAM)(void *vfm_memory_ctxt,int size, int align, bool inside_one_bank)
{
	void* allocated_ptr;
	t_uint32 phys_addr;

    OstTraceFiltInst3(TRACE_FLOW, "AllocESRAM - size %d - align 0x%x - inside_one_bank %d", size, align, inside_one_bank);

	allocated_ptr = mallocCM_MPC(size, align);
    if (allocated_ptr == NULL)
        NMF_PANIC("eSRAM allocation failed - no memory\n");

	if (!inside_one_bank)
	{
        OstTraceFiltInst0(TRACE_FLOW, "");
        OstTraceFiltInst2(TRACE_FLOW, "*** allocated_ptr phys_addr 0x%x (0x%x)",(t_uint32)GetPhysicallAddr(allocated_ptr),(t_uint32)allocated_ptr);
		return allocated_ptr;
	}

	phys_addr = (t_uint32)GetPhysicallAddr(allocated_ptr);
    OstTraceFiltInst3(TRACE_FLOW, "Buffer physical address = %x - %x (%p)", phys_addr, phys_addr + size - 1, (t_uint32)allocated_ptr);

	if ((phys_addr & ~(0x20000-1)) != ((phys_addr+size-1) & ~(0x20000-1)))
	{
		int size_to_next_bank_border;
		void* ptr = NULL;
		t_uint32 phys_addr1, phys_addr2;

		free((void*)allocated_ptr);
		size_to_next_bank_border = (0x20000 - (phys_addr & (0x20000-1))) & (0x20000-1);
		if (size_to_next_bank_border)
		{
			ptr = mallocCM_MPC(size_to_next_bank_border, align);
			phys_addr1 = (t_uint32)GetPhysicallAddr(ptr);
            OstTraceFiltInst3(TRACE_FLOW, "1:Search buffer physical address = %x - %x (%p)", phys_addr1, phys_addr1 + size_to_next_bank_border -1, (t_uint32)ptr);
		}
		allocated_ptr = mallocCM_MPC(size, align);
		phys_addr2 = (t_uint32)GetPhysicallAddr(allocated_ptr);
        OstTraceFiltInst3(TRACE_FLOW, "2:Search buffer physical address = %x - %x (%p)", phys_addr2, phys_addr2 + size -1, (t_uint32)allocated_ptr);
		if (size_to_next_bank_border)
			free(ptr);

	}


    if (allocated_ptr == NULL)
        NMF_PANIC("eSRAM allocation failed - no memory\n");

    OstTraceFiltInst0(TRACE_FLOW, "");
    OstTraceFiltInst2(TRACE_FLOW, "*** allocated_ptr phys_addr 0x%x (0x%x)",(t_uint32)GetPhysicallAddr(allocated_ptr),(t_uint32)allocated_ptr);
	return allocated_ptr;
}

void METH(AllocateMemory)(void *vfm_memory_ctxt, t_frameinfo *p_params, t_host_info *host, t_hamac_pipe *hamac_pipe)
{
	OstTraceFiltInst0(TRACE_API, ">>>>>> H264Enc ARM NMF :: AllocateMemory ");
	t_dec_buff *p_buff = host->buff;
	t_uint32 sww = p_params->common_frameinfo.pic_width;
	t_uint32 swh = p_params->common_frameinfo.pic_height;
	t_uint16 mbx = sww / 16;
	t_uint16 mby = swh / 16;

	p_buff->PicSizeInMbs = mbx * mby;

	p_buff->p_deblocking_paramv = (tps_h4d_param *)mallocCM(mbx * mby * sizeof(tps_h4d_param), SVA_MM_ALIGN_WORD);

	// Moved to DDR to allow big resolutions
	p_buff->addr_h264e_H4D_buffer = mallocCM(((mbx+2)*(mby+2)* 4 * 4 +15), 0xFF);

	p_buff->addr_ref_frame = mallocCM((sww*swh*3)/2, 0xFF);
	p_buff->addr_dest_buffer = mallocCM((sww*swh*3)/2, 0xFF);

	p_buff->addr_intra_refresh_buffer = mallocCM(((sww/16)*(swh/16)*4+15), 0xFF);

#ifndef __ndk5500_a0__
	p_buff->addr_motion_vector_buffer = mallocCM(((sww/16)*(swh/16)*8*2), 0xFF);
	if (1) {
		memset((t_uint8*)p_buff->addr_motion_vector_buffer, 0, (sww/16)*(swh/16)*8*2);
	}
    OstTraceFiltInst1(TRACE_FLOW, "1: p_deblocking_paramv = %x", (t_uint32)p_buff->p_deblocking_paramv);
	OstTraceFiltInst1(TRACE_FLOW, "2: addr_h264e_H4D_buffer = %x", (t_uint32)p_buff->addr_h264e_H4D_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "3: addr_ref_frame = %x", (t_uint32)p_buff->addr_ref_frame);
	OstTraceFiltInst1(TRACE_FLOW, "4: addr_dest_buffer = %x", (t_uint32)p_buff->addr_dest_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "5: addr_intra_refresh_buffer = %x", (t_uint32)p_buff->addr_intra_refresh_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "6: addr_motion_vector_buffer = %x", (t_uint32)p_buff->addr_motion_vector_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "7: addr_search_window_buffer = %x", (t_uint32)p_buff->addr_search_window_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "8: addr_h264e_rec_local = %x", (t_uint32)p_buff->addr_h264e_rec_local);
	OstTraceFiltInst1(TRACE_FLOW, "9: addr_h264e_cup_context = %x", (t_uint32)p_buff->addr_h264e_cup_context);
#else
	p_buff->addr_motion_vector_buffer = mallocCM(((sww/16+2)*(swh/16+2)*8+15), 0xFF);
	if (1) {
		memset((t_uint8*)p_buff->addr_motion_vector_buffer, 0, (sww/16+2)*(swh/16+2)*8+15);
	}
#endif

	// search window size for +/-32 pixel in vertical (was 6*16)

	//int size = 4*16*sfw; /* 16 * ( 1 + 1 (up) + 1 (down) + 1 (?) ) */
	//size = 6;
	//int size = (p_params->specific_frameinfo.SearchWindow / 16) * 2 + 1;
#ifndef _8500_V2_
	int size = ((swh == 1088 ? 16 : p_params->specific_frameinfo.SearchWindow) / 16) * 2 + 2;
#else
	int size = (p_params->specific_frameinfo.SearchWindow / 16) * 2 + 2;
#endif
	OstTraceFiltInst1(TRACE_FLOW, "Use height of %d MBs for search window", size);
	size *= 16*sww;

	p_buff->sw_size = size;
#ifndef __ndk5500_a0__
#ifndef _8500_V2_
	p_buff->addr_search_window_buffer = AllocESRAM(vfm_memory_ctxt,size, 0xFF, true);
#else
	p_buff->addr_search_window_buffer = AllocESRAM(vfm_memory_ctxt,size, 0xFF, false);
#endif
#endif

#ifndef __ndk5500_a0__
	if (!host->enable_hdtv)
	{
		OstTraceFiltInst0(TRACE_FLOW, "Allocating ESRAM Memory for RECONST buffer : FOR 720p FLOW");
		p_buff->addr_h264e_rec_local = AllocESRAM(vfm_memory_ctxt,(mbx*5*256*3)/2, 0xFF, false);
	}
	p_buff->addr_h264e_cup_context = AllocESRAM(vfm_memory_ctxt,7*8*mbx, 0xFF, false);
#endif

#ifdef __ndk5500_a0__
        // t_los_memory_handle handle = LOS_Alloc(bytes+1, 0xFF, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	p_buff->addr_external_cwi = (void*)0x40000000; // + sizeof(ts_t1xhv_vec_h264_param_inout));
	// p_buff->addr_sequence_info_buffer = (void*)((t_ulong_value)p_buff->addr_external_cwi + 0x1000);    /* 16*256 */
	//p_buff->addr_sequence_info_buffer =  AllocESRAM(vfm_memory_ctxt, 2*21*8, 0xFF);
	p_buff->addr_sequence_info_buffer = /* 0x150=2*21*8 = size addr_sequence_info_buffer */
		(void*)VFM_Alloc(vfm_memory_ctxt, 2*21*8, VFM_MEM_HWBUFFER_NONDSP, 0xFF, 0, __LINE__, (t_uint8 *)__FILE__);

	p_buff->addr_h264e_rec_local = (void*)((t_ulong_value)p_buff->addr_external_cwi + 0x1000);

	p_buff->addr_search_window_buffer = (void*)((t_ulong_value)p_buff->addr_h264e_rec_local + (mbx*5*256*3)/2);
	//p_buff->addr_h264e_cup_context = (void*)((t_ulong_value)p_buff->addr_search_window_buffer + size);
	p_buff->addr_h264e_cup_context = //AllocESRAM(vfm_memory_ctxt,240*mbx*mby, 0xFF);  /*240bytes/MB */
		VFM_Alloc(vfm_memory_ctxt, 240*mbx*mby, VFM_MEM_HWBUFFER_NONDSP, 0xFF, 0, __LINE__, (t_uint8 *)__FILE__);
#endif

    OstTraceFiltInst1(TRACE_FLOW, "1: p_deblocking_paramv = %x", (t_uint32)p_buff->p_deblocking_paramv);
	OstTraceFiltInst1(TRACE_FLOW, "2: addr_h264e_H4D_buffer = %x", (t_uint32)p_buff->addr_h264e_H4D_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "3: addr_ref_frame = %x", (t_uint32)p_buff->addr_ref_frame);
	OstTraceFiltInst1(TRACE_FLOW, "4: addr_dest_buffer = %x", (t_uint32)p_buff->addr_dest_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "5: addr_intra_refresh_buffer = %x", (t_uint32)p_buff->addr_intra_refresh_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "6: addr_motion_vector_buffer = %x", (t_uint32)p_buff->addr_motion_vector_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "7: addr_search_window_buffer = %x", (t_uint32)p_buff->addr_search_window_buffer);
	OstTraceFiltInst1(TRACE_FLOW, "8: addr_h264e_rec_local = %x", (t_uint32)p_buff->addr_h264e_rec_local);
	OstTraceFiltInst1(TRACE_FLOW, "9: addr_h264e_cup_context = %x", (t_uint32)p_buff->addr_h264e_cup_context);
	OstTraceFiltInst1(TRACE_FLOW, "8: addr_h264e_rec_local = %x", (t_uint32)p_buff->addr_h264e_rec_local);
    OstTraceFiltInst1(TRACE_FLOW, "9: addr_h264e_cup_context = %x", (unsigned int)p_buff->addr_h264e_cup_context);
    OstTraceFiltInst0(TRACE_API, "<<<<<< H264Enc ARM NMF :: AllocateMemory ");
}

void FreeMemory(void *vfm_memory_ctxt, t_host_info *host, t_hamac_pipe *hamac_pipe)
{
	OstTraceInt0(TRACE_API, ">>>>>> H264Enc ARM NMF :: FreeMemory");
	t_dec_buff *p_buff = host->buff;
	free((void*)p_buff->p_deblocking_paramv);
	free((void*)p_buff->addr_h264e_H4D_buffer);
	free((void*)p_buff->addr_ref_frame);
	free((void*)p_buff->addr_dest_buffer);
	free((void*)p_buff->addr_intra_refresh_buffer);
	free((void*)p_buff->addr_motion_vector_buffer);
#ifndef __ndk5500_a0__
	free((void*)p_buff->addr_search_window_buffer);
	if (!host->enable_hdtv)
	{
		OstTraceInt0(TRACE_FLOW, "Freeing ESRAM Memory for RECONST buffer : FOR 720p FLOW");
		free((void*)p_buff->addr_h264e_rec_local);
	}
	free((void*)p_buff->addr_h264e_cup_context);
#endif
#ifdef __ndk5500_a0__
	free((void*)p_buff->addr_sequence_info_buffer);
	free((void*)p_buff->addr_h264e_cup_context);
#if 0
	//	if (!host->enable_hdtv)
	free((void*)p_buff->addr_h264e_rec_local);
	free((void*)p_buff->addr_external_cwi);
#endif
#endif
	OstTraceInt0(TRACE_API, "<<<<<< H264Enc ARM NMF :: FreeMemory");
}

void init_decoder(t_dec *dec, void *vfm_memory_ctxt)
{
	OstTraceInt0(TRACE_API, ">>>>>> H264Enc ARM NMF :: init_decoder");
	dec->host.buff = &dec->buf;
	//hamac_pipe_init(&dec->hamac_pipe);
}

void close_decoder(t_dec *dec, void *vfm_memory_ctxt)
{
	OstTraceInt0(TRACE_API, ">>>>>> H264Enc ARM NMF :: close_decoder");
	FreeMemory(vfm_memory_ctxt, &dec->host, &dec->hamac_pipe);
	OstTraceInt0(TRACE_API, "<<<<<< H264Enc ARM NMF :: close_decoder");
}

t_nmf_error METH(construct)(void)
{
	buff_inited = 0;
	async_init_done = 0;

	picture_number = 0;
	frame_number = 0;
	FramePictureCounter = 0;
	frame = 0;

	timestamp = 0;

	//lastBPAUts = 0;

	header_size = 0;
	no_headers = 0;
	sps_id = 0;

	idx_fifo_in_push = 0;
	idx_fifo_in_pull = 0;
	idx_fifo_out_push = 0;
	idx_fifo_out_pull = 0;
	lock = 0;

	return NMF_OK;
}

void METH(destroy)(void)
{
	OstTraceFiltInst0(TRACE_API, ">>>>>> H264Enc ARM NMF :: destroy");
    if (vfm_memory_ctxt)
    {
		OstTraceFiltInst0(TRACE_FLOW, "About to call HamacDeallocParameter");
        HamacDeallocParameter(vfm_memory_ctxt, &hamac_param);
    }
	if (vfm_memory_ctxt && buff_inited)
	{
		close_decoder(&dec_static, vfm_memory_ctxt);
	}
	OstTraceFiltInst0(TRACE_API, ">>>>>> H264Enc ARM NMF :: destroy");
}

void METH(init)(void *mem_ctxt)
{
    OstTraceFiltInst0(TRACE_FLOW, "[ENCODER] > init()");

	my_vfm_memory_ctxt = vfm_memory_ctxt = mem_ctxt;
	init_decoder(&dec_static, vfm_memory_ctxt);
	OstTraceFiltInst0(TRACE_FLOW, "About to call HamacAllocParameter");
	memset(&hamac_param,0,sizeof(t_hamac_param));
	HamacAllocParameter(vfm_memory_ctxt, &hamac_param);
	async_init_done = 1;

    pendingCommandAck.pendingCommandAck(0);

    OstTraceFiltInst0(TRACE_FLOW, "[ENCODER] < init()");
}

void METH(close)(void)
{
    pendingCommandAck.pendingCommandAck(0);
}

extern "C" t_uint8* bitbuf;

//#define max(a,b) ((a)>(b)?(a):(b))

t_sint32 CeilLog2(t_sint32 val)
{
	t_sint32 cl2 = 1;
	for(cl2 = 0; cl2 < 32; cl2++)
		if ((1 << cl2) > val)
			return cl2;
	return 0;
}

#if defined __ndk8500_a0__ || defined __ndk8500_b0__ || defined __ndk5500_a0__
const t_uint32 LevelLimits[16][6] = {
	{ 10,   1485,   99,      64,    175, 2}, /* 1.0  [0] */
	{101,   1485,   99,     128,    350, 2}, /* 1.0b [1] */
	{ 11,   3000,  396,     192,    500, 2}, /* 1.1  [2] */
	{ 12,   6000,  396,     384,   1000, 2}, /* 1.2  [3] */
	{ 13,  11880,  396,     768,   2000, 2}, /* 1.3  [4] */
	{ 20,  11880,  396,    2000,   2000, 2}, /* 2.0  [5] */
	{ 21,  19800,  792,    4000,   4000, 2}, /* 2.1  [6] */
	{ 22,  20250, 1620,    4000,   4000, 2}, /* 2.2  [7] */
	{ 30,  40500, 1620,   10000,  10000, 2}, /* 3.0  [8] */
	{ 31, 108000, 3600,   14000,  14000, 4}, /* 3.1  [9] */
	{ 32, 216000, 5120,   20000,  20000, 4}, /* 3.2  [10] */
	{ 40, 245760, 8192,   20000,  25000, 4}, /* 4.0  [11] */
	{ 41, 245760, 8192,   50000,  62500, 2}, /* 4.1  [12] */
	{ 42, 522240, 8704,   50000,  62500, 2}, /* 4.2  [13] */
	{ 50, 589824, 22080, 135000, 135000, 2}, /* 5.0  [14] */
	{ 51, 983040, 36864, 240000, 240000, 2}  /* 5.1  [15] */
};
extern "C" t_uint16 getIndexFromLevel(t_uint16 level);
/*!
 *************************************************************************************
 * \brief
 *    t_uint32 ComputeMaxBitSizePerAU (void)
 *
 * \note
 *    This function compute the max size of a whole AU
 *    (passed to the BRC in order to limit the size of pictures)
 *
 *************************************************************************************
 */
t_uint32 ComputeMaxBitSizePerAU(t_frameinfo *p_params)
{
	t_uint32 MaxBitSizePerAU;
	t_uint16 i;
	t_uint32 PicSizeInMbs = (p_params->common_frameinfo.pic_width / 16) * (p_params->common_frameinfo.pic_height / 16);
	t_uint32 framerate = p_params->specific_frameinfo.FrameRate;
	static t_uint32 number = 0;

	i = getIndexFromLevel(p_params->specific_frameinfo.EncoderLevel);

	if (number++ == 0)
		MaxBitSizePerAU = (384 * PicSizeInMbs) / LevelLimits[i][5];
	else {
		/* NZ: this 1000 is to take into account fractional frame rates */
		MaxBitSizePerAU = 1000*(384 * LevelLimits[i][1] / ((framerate>>16)*1000) ) / LevelLimits[i][5];
	}
	return MaxBitSizePerAU<<3; /* NZ: in bits */
}
#endif

void METH(get_headers)(t_frameinfo* p_params, void* buffer1, t_uint16* size1, void* buffer2, t_uint16* size2, t_uint16 no_headers_stream)
{
	OstTraceFiltInst0(TRACE_API, ">>>>>> H264ENC ARM NMF :: get_headers");
	while(async_init_done == 0){;} /* patch by MC to make sure that async i/F init has been executed */

	no_headers = no_headers_stream;

	pic_parameter_set_rbsp_t active_pps;
	seq_parameter_set_rbsp_t active_sps;
	NALU_t *spsNAL;
	NALU_t *ppsNAL;
	t_uint32 len_sps;
	char buffer[4]= {0x00, 0x00, 0x00, 0x01};
	ts_t1xhv_vec_h264_param_in in_parameters;

	in_parameters.pic_order_cnt_type = p_params->specific_frameinfo.PicOrderCntType;
	in_parameters.log2_max_frame_num_minus4 = 0x0000;
	in_parameters.window_width = p_params->common_frameinfo.pic_width;
	in_parameters.window_height = (p_params->common_frameinfo.pic_height / 16) * 16;
	in_parameters.use_constrained_intra_flag = p_params->specific_frameinfo.UseConstrainedIntraPred;
	in_parameters.TransformMode = p_params->specific_frameinfo.TransformMode;
	in_parameters.bit_rate = Endianess(p_params->specific_frameinfo.Bitrate);
	//+ code for CR 332873
	if (p_params->specific_frameinfo.CpbBufferSize)
	{
		in_parameters.CpbBufferSize = Endianess(p_params->specific_frameinfo.CpbBufferSize);
	}
	else
	{
		in_parameters.CpbBufferSize = Endianess(p_params->specific_frameinfo.Bitrate);
	}
	//- code for CR 332873

	in_parameters.framerate = p_params->specific_frameinfo.FrameRate >> 6;

	FillParameterSetStructures(&active_sps, &active_pps, p_params->specific_frameinfo.EncoderLevel, &p_params->specific_frameinfo, &in_parameters);
	active_sps.seq_parameter_set_id = sps_id;
	if(buffer2)
		sps_id = (sps_id + 1) & 0x1f;//Coverity Warning Fix
	memcpy(buffer1, buffer, 4);

	bitbuf = (t_uint8*)buffer1;
	bitbuf += 4;
	spsNAL = GenerateSeq_parameter_set_NALU(&active_sps);
	len_sps = spsNAL->len;
	bitbuf += spsNAL->len;

	if (p_params->specific_frameinfo.OutFileMode)
		*(((t_uint8*)buffer1)+3) = spsNAL->len;

	*size1 = len_sps + 4;
	OstTraceInt1(TRACE_FLOW, "SPS size = %d", *size1);

	if (buffer2)
		bitbuf = (t_uint8*)buffer2;
	if (buffer2)
		memcpy(((t_uint8*)buffer2), buffer, 4);
	else
		memcpy(((t_uint8*)buffer1)+4+spsNAL->len, buffer, 4);
	bitbuf += 4;
	ppsNAL = GeneratePic_parameter_set_NALU(&active_pps);

	if (p_params->specific_frameinfo.OutFileMode)
	{
        if (buffer2)
			*(((t_uint8*)buffer2)+3) = ppsNAL->len;
		else
			*(((t_uint8*)buffer1)+4+len_sps+3) = ppsNAL->len;
    }
	OstTraceFiltInst1(TRACE_FLOW, "PPS size = %d", ppsNAL->len + 4);
	if (buffer2)
		*size2 = ppsNAL->len + 4;
	else
		*size1 += ppsNAL->len + 4;
	{
		int i;
		for(i = 0; i < *size1; i++)
        {
            OstTraceFiltInst1(TRACE_FLOW, "%02x", *((t_uint8*)buffer1 +i));
        }
		if (buffer2)
			for(i = 0; i < *size2; i++)
            {
                OstTraceFiltInst1(TRACE_FLOW, "%02x", *((t_uint8*)buffer2 +i));
	}
	}

	header_size = len_sps + 4 + ppsNAL->len + 4;

    pendingCommandAck.pendingCommandAck(0);
    OstTraceFiltInst0(TRACE_API, "<<<<<<< H264ENC ARM NMF :: get_headers");
}

extern "C" void InitSEIMessages(void);
extern "C" seq_parameter_set_rbsp_t* Gsp_sps;
extern "C" ts_t1xhv_vec_h264_param_in* Gsp_img;
extern "C" ts_t1xhv_vec_h264_param_inout* Gsp_inout;
extern "C" t_uint16 sei_message_payloadSize;

/* Initialize the CIR table ***/
/* We will save it in 8 bits **/
/* Standard specifies 132 as maximum value */

#define CLIP255(val) (((val)<(0))?(0):(((val)>(132))?(132):(val)))

void METH(CirInit)(t_uint16 seed, t_host_info *host, t_uint16 CirMbNum,t_uint16 IntraType) {
  t_dec_buff *p_buff = host->buff;
  t_uint16 *cirIndex = (t_uint16*)p_buff->addr_intra_refresh_buffer;
  t_sint16 lfsr = seed;
  t_uint16 bit;
  t_sint32 period = 0;
  t_sint16 lfsr_mod=0;
  
  do
  {
    /* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);
    if(CirMbNum !=0)
     lfsr_mod = lfsr % CirMbNum;
    lfsr_mod = CLIP255(lfsr_mod);
    *((t_uint16*)cirIndex + period) =   IntraType ? lfsr_mod : CLIP255(lfsr);
    period++;   
  } while(period<p_buff->PicSizeInMbs/*[Gsp_input->NumberLayer-1]*/);
}

void METH(input)(void *p_frame, t_frameinfo v_params, t_uint32 nFlags)
{
	t_uint32 num, den;
	t_uint32 estimated_sei_size = 0;
	static seq_parameter_set_rbsp_t active_sps;
	Gsp_sps = &active_sps;
	t_uint32 MaxSumNumBitsInNALU;
	t_hamac_param* h_param;
	t_frameinfo *p_params = &(v_params);
	t_uint32 last_cir = 0;
    OstTraceFiltInst1(TRACE_FLOW, "Input called lock = %d", lock);
	lock = 1;

	h_param = &cmd_fifo[idx_fifo_in_push];

#ifndef __ndk5500_a0__
	h_param->addr_in_frame_buffer = hamac_param.addr_in_frame_buffer + idx_fifo_in_push;
	h_param->addr_out_frame_buffer = hamac_param.addr_out_frame_buffer + idx_fifo_in_push;
	h_param->addr_internal_buffer = hamac_param.addr_internal_buffer;
	h_param->addr_in_bitstream_buffer = hamac_param.addr_in_bitstream_buffer + idx_fifo_in_push;
	h_param->addr_out_bitstream_buffer = hamac_param.addr_out_bitstream_buffer;
	h_param->addr_in_parameters = hamac_param.addr_in_parameters + idx_fifo_in_push;
	h_param->addr_out_parameters = hamac_param.addr_out_parameters + idx_fifo_in_push;
	h_param->addr_in_frame_parameters = hamac_param.addr_in_frame_parameters + idx_fifo_in_push;
	h_param->addr_out_frame_parameters = hamac_param.addr_in_frame_parameters + (idx_fifo_in_push + 1) % CMD_FIFO_SIZE;
	h_param->addr_bitstream_buf_struct = hamac_param.addr_bitstream_buf_struct + idx_fifo_in_push;
#endif

	h_param->info = p_params->specific_frameinfo;
	h_param->IntraPeriod = p_params->specific_frameinfo.IntraPeriod;
	h_param->IDRIntraEnable = p_params->specific_frameinfo.IDRIntraEnable;
	h_param->flags = nFlags;
	h_param->p_input_buffer = p_frame;
	h_param->in_size = (p_params->specific_frameinfo.sfw * p_params->specific_frameinfo.sfh * 3 ) / 2;
	h_param->output_buffer = p_output_buffer[idx_fifo_out_pull];
	dec_static.host.buff->bs_size = output_size[idx_fifo_out_pull];
	OutFileMode = p_params->specific_frameinfo.OutFileMode;

//+ code for step 2 of CR 332873
	h_param->config_change = (!buff_inited) ||
		(p_params->specific_frameinfo.FrameRate != last_framerate) ||
		((p_params->specific_frameinfo.Bitrate != last_bitrate) && p_params->specific_frameinfo.BrcType);
	if (h_param->config_change)
	{

		OstTraceFiltInst0(TRACE_FLOW, "@@@@@@@@@@@@@@@@@@@@@ CONFIG CHANGE");
		OstTraceFiltInst1(TRACE_FLOW, "%d", buff_inited);
		OstTraceFiltInst1(TRACE_FLOW, "%d", last_framerate);
		OstTraceFiltInst1(TRACE_FLOW, "%d", last_bitrate);
		OstTraceFiltInst1(TRACE_FLOW, "%d", p_params->specific_frameinfo.FrameRate);
		OstTraceFiltInst1(TRACE_FLOW, "%d", p_params->specific_frameinfo.Bitrate);
		OstTraceFiltInst1(TRACE_FLOW, "%d", p_params->specific_frameinfo.BrcType);
		last_framerate = p_params->specific_frameinfo.FrameRate;
		last_bitrate = p_params->specific_frameinfo.Bitrate;
	}
//- code for step 2 of CR 332873
	if (!buff_inited)
	{
		last_theorical_timestamp = p_params->common_frameinfo.nTimeStampL;
		OstTraceFiltInst3(TRACE_FLOW, "First time stamp %lu (%lu/%lu)", p_params->common_frameinfo.nTimeStampL, picture_number, timestamp);
	}
	else
	{
		t_uint32 delta;
		t_uint32 new_theorical_timestamp;
		t_uint32 periods;
		t_sint32 jitter;
		// FIXME: Improve accuracy by using 64 bits arithmetic */
		delta = (1000000*255) / (p_params->specific_frameinfo.FrameRate / 255);
		/* Handle case where frames are skipped at grab level. Framerate change is handled as well */
		periods = ((p_params->common_frameinfo.nTimeStampL + delta / 2) - last_theorical_timestamp) / delta;
		new_theorical_timestamp = last_theorical_timestamp + periods * delta;
		jitter = (t_sint32)p_params->common_frameinfo.nTimeStampL - (t_sint32)new_theorical_timestamp;
		last_theorical_timestamp = new_theorical_timestamp;
		timestamp += periods;
		OstTraceFiltInst3(TRACE_FLOW, "Time stamp %lu, Theorical time stamp %lu, periods = %lu, ", p_params->common_frameinfo.nTimeStampL, new_theorical_timestamp, periods);
		OstTraceFiltInst4(TRACE_FLOW, "jitter = %ld = %ld%% (%lu/%lu)", jitter, 100 * jitter / (t_sint32)delta, picture_number, timestamp);
	}

#ifndef __ndk5500_a0__
	dec_static.host.enable_hdtv = p_params->specific_frameinfo.LowComplexity && !p_params->specific_frameinfo.EncoderComplexity;
    OstTraceFiltInst2(TRACE_FLOW, "FW used: Camcorder=%d, Visio=%d", dec_static.host.enable_hdtv, !dec_static.host.enable_hdtv);
#endif

	if (!buff_inited) {
		AllocateMemory(vfm_memory_ctxt, p_params, &dec_static.host, &dec_static.hamac_pipe);
		if (p_params->specific_frameinfo.HrdSendMessages == 2) {
			InitSEIMessages();
		}
		NonVCLNALUSize = 0;
		memset(h_param->addr_in_frame_parameters, 0, sizeof(*h_param->addr_in_frame_parameters));
#ifndef __ndk5500_a0__
		h_param->addr_in_frame_parameters->timestamp_old = -1;
#endif
		skip_current = skip_next = 0;
		mv_toggle = 0;
	} else {
		if (!skip_current)
		{
			void* tmp = dec_static.buf.addr_ref_frame;
			dec_static.buf.addr_ref_frame = dec_static.buf.addr_dest_buffer;
			dec_static.buf.addr_dest_buffer = tmp;
		}
		header_size = 0;
	}
	if((last_cir != p_params->specific_frameinfo.CirMbNum)&&(p_params->specific_frameinfo.IntraRefreshType>1))//Refresh Type >1 means CIR
	{
		CirInit(0xACE1u,&dec_static.host,p_params->specific_frameinfo.CirMbNum,p_params->specific_frameinfo.IntraRefreshType); /* Init LSFR (random polynom)*/
		last_cir = p_params->specific_frameinfo.CirMbNum;
//#ifdef _CACHE_OPT_				
		//VFM_CacheInvalidate(vfm_memory_ctxt, (t_uint8*)dec_static.host.buff->addr_intra_refresh_buffer, dec_static.host.buff->PicSizeInMbs);
//#endif
	}

	dec_static.buf.addr_source_buffer = h_param->p_input_buffer;
	dec_static.buf.addr_bitstream_buffer = h_param->output_buffer;
	OstTraceFiltInst0(TRACE_FLOW, "H264Enc ARM NMF: About to call HamacFillParamIn");
	HamacFillParamIn(vfm_memory_ctxt, &dec_static, 0, h_param);

	if (1) {
		t_uint16 iter = 0;
		den = 1;
		double ret;
		double in = p_params->specific_frameinfo.FrameRate;
		in = in / 65536;
		while (iter < 4) {
			ret = in - (t_uint32)in;
			if (ret==0)
				break;
			else {
				in *= 10;
			}
			iter++;
			den *= 10;
		}
		num = (t_uint32) in;
        OstTraceFiltInst2(TRACE_FLOW, "FrameRate = %d / %d", num, den);
	}

	Gsp_img = h_param->addr_in_parameters;
	Gsp_inout = h_param->addr_in_frame_parameters;
//+ code for step 2 of CR 332873
    memset(h_param->addr_in_parameters, 0, sizeof(h_param->addr_in_parameters));
//- code for step 2 of CR 332873
	/* constant params */
	h_param->addr_in_parameters->picture_coding_type = I_SLICE;
	h_param->addr_in_parameters->frame_width = p_params->specific_frameinfo.sfw;
	h_param->addr_in_parameters->frame_height = (p_params->specific_frameinfo.sfh / 16) * 16;
	h_param->addr_in_parameters->window_width = p_params->common_frameinfo.pic_width;
	h_param->addr_in_parameters->window_height = (p_params->common_frameinfo.pic_height / 16) * 16;
	h_param->addr_in_parameters->window_horizontal_offset = p_params->specific_frameinfo.sho;
	h_param->addr_in_parameters->window_vertical_offset = p_params->specific_frameinfo.svo;
	h_param->addr_in_parameters->FirstPictureInSequence = !buff_inited;
	h_param->addr_in_parameters->frame_num = 0;

	/* the following are from the configuration file */
	h_param->addr_in_parameters->use_constrained_intra_flag = p_params->specific_frameinfo.UseConstrainedIntraPred;
	h_param->addr_in_parameters->slice_size_type = p_params->specific_frameinfo.SliceMode;
	h_param->addr_in_parameters->slice_byte_size = p_params->specific_frameinfo.ByteSliceSize;
	h_param->addr_in_parameters->slice_mb_size = p_params->specific_frameinfo.MbSliceSize;
	h_param->addr_in_parameters->intra_refresh_type = 0;
	h_param->addr_in_parameters->air_mb_num = 0;
	memset(h_param->addr_in_parameters->slice_loss_first_mb, 0, 8*sizeof(t_ushort_value));
	memset(h_param->addr_in_parameters->slice_loss_mb_num, 0, 8*sizeof(t_ushort_value));
	h_param->addr_in_parameters->MaxSumNumBitsInNALU = 0;

	/* the following are used to encode the Slice Header */
	h_param->addr_in_parameters->pic_order_cnt_type = p_params->specific_frameinfo.PicOrderCntType;
	h_param->addr_in_parameters->log2_max_frame_num_minus4 = 0;

	/* the following parameters are for future use */
	h_param->addr_in_parameters->disable_deblocking_filter_idc = p_params->specific_frameinfo.disableH4D;
	h_param->addr_in_parameters->slice_alpha_c0_offset_div2 = 0;
	h_param->addr_in_parameters->slice_beta_offset_div2 = 0;

	/* the following are specific for use with the rate-controller */
	h_param->addr_in_parameters->brc_type = p_params->specific_frameinfo.BrcType;
	/* FIXME: Will not work with pipe */
	//h_param->addr_in_parameters->lastBPAUts = Endianess(lastBPAUts);
	//h_param->addr_in_parameters->CpbBufferSize = Endianess(128000);
//+ code for step 2 of CR 332873
	h_param->addr_in_parameters->CpbBufferSize = Endianess(p_params->specific_frameinfo.CpbBufferSize);
//- code for step 2 of CR 332873
	h_param->addr_in_parameters->bit_rate = Endianess(p_params->specific_frameinfo.Bitrate);
	MaxSumNumBitsInNALU = ComputeMaxBitSizePerAU(p_params);
	h_param->addr_in_parameters->MaxSumNumBitsInNALU = Endianess(MaxSumNumBitsInNALU);
	h_param->addr_in_parameters->intra_refresh_type = p_params->specific_frameinfo.IntraRefreshType;
	h_param->addr_in_parameters->air_mb_num = p_params->specific_frameinfo.AirMbNum;
	h_param->addr_in_parameters->cir_mb_num = p_params->specific_frameinfo.CirMbNum;
	h_param->addr_in_parameters->TransformMode = p_params->specific_frameinfo.TransformMode;
	h_param->addr_in_parameters->encoder_complexity = p_params->specific_frameinfo.EncoderComplexity;
#ifndef __ndk5500_a0__
#ifndef _8500_V2_
	h_param->addr_in_parameters->searchWin = (p_params->common_frameinfo.pic_height == 1088) ? 16 : p_params->specific_frameinfo.SearchWindow;
#else
	h_param->addr_in_parameters->searchWin = p_params->specific_frameinfo.SearchWindow;
#endif
	h_param->addr_in_parameters->LowComplexity = p_params->specific_frameinfo.LowComplexity;
	h_param->addr_in_parameters->ForceIntrapred = p_params->specific_frameinfo.ForceIntraPredMode;
	h_param->addr_in_parameters->ProfileIDC = p_params->specific_frameinfo.ProfileIDC;
	h_param->addr_in_parameters->VBRConfig = p_params->specific_frameinfo.VBRConfig;
	h_param->addr_in_parameters->FrameRate.num = Endianess(num);
	h_param->addr_in_parameters->FrameRate.den = Endianess(den);
//+ code for step 2 of CR 332873
	//memset(h_param->addr_in_parameters->reserved2, 0, sizeof(h_param->addr_in_parameters->reserved2));
//- code for step 2 of CR 332873
	h_param->addr_in_parameters->framerate = p_params->specific_frameinfo.FrameRate >> 6;
	h_param->addr_in_parameters->QPISlice = p_params->specific_frameinfo.QPISlice;
	h_param->addr_in_parameters->QPPSlice = p_params->specific_frameinfo.QPPSlice;
#else
	h_param->addr_in_parameters->sampling_mode = p_params->specific_frameinfo.SamplingMode;
	h_param->addr_in_parameters->framerate_num = p_params->specific_frameinfo.FrameRate >> 16;
	h_param->addr_in_parameters->framerate_den = 1;
#endif
	h_param->addr_in_parameters->NonVCLNALUSize = 0;
	h_param->addr_in_parameters->NALfinal_arrival_time = 0;
	h_param->addr_in_parameters->mv_toggle = mv_toggle;
	h_param->addr_in_parameters->timestamp = Endianess(timestamp);

//+ code for step 2 of CR 332873
#ifndef	__ndk5500_a0__
	h_param->addr_in_parameters->CBR_simplified_algo = p_params->specific_frameinfo.CBR_simplified_algo;
	h_param->addr_in_parameters->CBR_clipped = p_params->specific_frameinfo.CBR_clipped;
	h_param->addr_in_parameters->CBR_clipped_min_QP = p_params->specific_frameinfo.CBR_clipped_min_QP;
	h_param->addr_in_parameters->CBR_clipped_max_QP = p_params->specific_frameinfo.CBR_clipped_max_QP;
	h_param->addr_in_parameters->BRC_dynamic_change = h_param->config_change & buff_inited;
#endif
//- code for step 2 of CR 332873
	mv_toggle = !mv_toggle;

	if (frame == 16)
		frame = 0;
//+ code for step 2 of CR 332873
	/* Force an intra in case of config change or change scene detection */
	//+ code change for CR 361667
	if (h_param->config_change || p_params->specific_frameinfo.ForceIntra)
		frame_number = 0;
//- code change for CR 361667
//- code for step 2 of CR 332873
	OstTraceFiltInst1(TRACE_FLOW, "Host_Encoder : Input : p_params->specific_frameinfo.ForceIntra : %d",p_params->specific_frameinfo.ForceIntra);
	if ((frame_number % p_params->specific_frameinfo.IntraPeriod) == 0) {
		//+ code change for CR 361667
		h_param->addr_in_parameters->idr_flag = p_params->specific_frameinfo.IDRIntraEnable || h_param->config_change || p_params->specific_frameinfo.ForceIntra; // code change for step 2 of CR 332873
		//- code change for CR 361667
		if (p_params->specific_frameinfo.IDRIntraEnable == 1) {
			frame = 0;
		}
	}
	else {
		h_param->addr_in_parameters->idr_flag = 0;
	}

	h_param->addr_in_parameters->frame_num = Endianess(frame);

	if ((frame_number % p_params->specific_frameinfo.IntraPeriod) == 0) {
		h_param->addr_in_parameters->picture_coding_type = I_SLICE;
#ifdef __ndk5500_a0__
		if (!p_params->specific_frameinfo.BrcType)
			h_param->addr_in_parameters->quant = p_params->specific_frameinfo.QPISlice;
#endif
	} else {
		h_param->addr_in_parameters->picture_coding_type = P_SLICE;
#ifdef __ndk5500_a0__
		if (!p_params->specific_frameinfo.BrcType)
			h_param->addr_in_parameters->quant = p_params->specific_frameinfo.QPPSlice;
#endif
	}

	if (!buff_inited) {
#ifdef __ndk5500_a0__
		h_param->addr_in_parameters->quant = p_params->specific_frameinfo.QPISlice;
#endif
	}
//+ code for step 2 of CR 332873
#ifndef __ndk5500_a0__
    h_param->addr_in_parameters->MVC_encoding = 0;
    h_param->addr_in_parameters->MVC_view = 0;
    h_param->addr_in_parameters->MVC_anchor_picture_flag = 0;

    memset(h_param->addr_in_parameters->reserved3, 0, sizeof(h_param->addr_in_parameters->reserved3));
#endif
//- code for step 2 of CR 332873
	h_param->frame_number = frame_number;
	h_param->FramePictureCounter = FramePictureCounter;
	h_param->frame = frame;
	h_param->picture_number = picture_number;

	frame_number++;
	FramePictureCounter++;
	frame++;
	picture_number++;

//+ code for step 2 of CR 332873
	if (h_param->config_change && !no_headers) {
//- code for step 2 of CR 332873
		pic_parameter_set_rbsp_t active_pps;
		NALU_t *spsNAL;
		NALU_t *ppsNAL;
		t_uint32 len_sps, len_pps;
		char buffer[4]= {0x00, 0x00, 0x00, 0x01};

		FillParameterSetStructures(&active_sps, &active_pps, p_params->specific_frameinfo.EncoderLevel, &p_params->specific_frameinfo, h_param->addr_in_parameters);
		memcpy(h_param->output_buffer, buffer, 4);

		bitbuf = (t_uint8*)h_param->output_buffer;

		bitbuf += 4;
		spsNAL = GenerateSeq_parameter_set_NALU(&active_sps);
		len_sps = spsNAL->len;
		bitbuf += len_sps;

		if (p_params->specific_frameinfo.OutFileMode)
			*(((t_uint8*)h_param->output_buffer)+3) = len_sps;

		memcpy(((t_uint8*)h_param->output_buffer)+4+len_sps, buffer, 4);

		bitbuf += 4;
		ppsNAL = GeneratePic_parameter_set_NALU(&active_pps);
		len_pps = ppsNAL->len;
		bitbuf += len_pps;

		if (p_params->specific_frameinfo.OutFileMode)
			*(((t_uint8*)h_param->output_buffer)+4+len_sps+3) = ppsNAL->len;

		header_size = len_sps + 4 + len_pps + 4;
        OstTraceFiltInst1(TRACE_FLOW, "SPS size %d", len_sps + 4);
        OstTraceFiltInst1(TRACE_FLOW, "PPS size %d", len_pps + 4);
	}

#if 0
	if (0) { //p_params->specific_frameinfo.HrdSendMessages == 2)

		NALU_t *seiNAL;
		t_uint32 len_sei;
		char buffer[4]= {0x00, 0x00, 0x00, 0x01};

		memcpy(((t_uint8*)h_param->output_buffer) + header_size, buffer, 4);

		bitbuf = ((t_uint8*)h_param->output_buffer) + header_size;
		bitbuf += 4;
		seiNAL = GenerateSEI_NALU_(!buff_inited, picture_number - 1);
		len_sei = seiNAL->len + 4;

		header_size += len_sei;
        OstTraceFiltInst1(TRACE_FLOW, "SEI size %d", len_sei);
	}
#endif

	sei_present = (p_params->specific_frameinfo.HrdSendMessages == 2);

	if (sei_present)
	{
		//InitSEIio(SEIio);
		estimated_sei_size = estimate_sei_size(h_param->addr_in_parameters->FirstPictureInSequence) / 8;
		NonVCLNALUSize += estimated_sei_size;
        OstTraceFiltInst1(TRACE_FLOW, "Estimated SEI size %d", estimated_sei_size);
	}

	h_param->addr_in_parameters->NonVCLNALUSize = Endianess((header_size + estimated_sei_size) * 8);

	if (no_headers)
		header_size = 0;

	if (1) //header_size)
	{
		t_uint32 start_addr, aligned_addr;

		start_addr = (t_uint32) GetPhysicallAddr(dec_static.host.buff->addr_bitstream_buffer);
		start_addr += header_size + sei_present ? MAX_SEI_SIZE : 0;
		aligned_addr = start_addr & 0xFFFFFFF0;

		h_param->addr_in_bitstream_buffer->addr_bitstream_start = Endianess(aligned_addr);
		h_param->addr_in_bitstream_buffer->bitstream_offset = Endianess((start_addr - aligned_addr) * 8);

		h_param->addr_bitstream_buf_struct->addr_window_start = Endianess(aligned_addr);
#ifdef __ndk5500_a0__
		printf ("old bitstream_start 0x%x, new bitstream_start 0x%x header_size:%d bs_size:%d\n",
				h_param->addr_in_parameters->addr_output_bitstream_start, Endianess(aligned_addr),
				header_size, dec_static.host.buff->bs_size);
		//Update addr_output_bitstream_start in hva task descriptor...
		h_param->addr_in_parameters->addr_output_bitstream_start = Endianess(aligned_addr);
		// h_param->addr_in_parameters->addr_output_bitstream_end = Endianess(aligned_addr + dec_static.host.buff->bs_size);
		h_param->addr_in_parameters->addr_output_bitstream_end = Endianess(aligned_addr + dec_static.host.buff->bs_size);
		h_param->addr_in_parameters->bitstream_offset = Endianess((start_addr - aligned_addr) * 8);
#endif
	}

	h_param->header_size = header_size;

	buff_inited = 1;

	dump_struct("vec_h264_param_in", picture_number - 1, h_param->addr_in_parameters, sizeof(*h_param->addr_in_parameters));
#ifndef __ndk5500_a0__
	dump_struct("vec_h264_param_inout", picture_number - 1, h_param->addr_in_frame_parameters, sizeof(*h_param->addr_in_frame_parameters));
#endif

	idx_fifo_out_pull = (idx_fifo_out_pull + 1) % CMD_FIFO_SIZE;
	idx_fifo_in_push = (idx_fifo_in_push + 1) % CMD_FIFO_SIZE;

	OstTraceFiltInst0(TRACE_FLOW, "H264Enc ARM NMF: About to call call_dsp API");
	call_dsp(vfm_memory_ctxt, h_param);

	lock = 0;
}

void METH(output_buffer)(void *ptr, t_uint32 size)
{
    OstTraceFiltInst1(TRACE_FLOW, "output_buffer called lock = %d", lock);
	lock = 1;
	p_output_buffer[idx_fifo_out_push] = ptr;
	output_size[idx_fifo_out_push] = size;
	idx_fifo_out_push = (idx_fifo_out_push + 1) % CMD_FIFO_SIZE;
	lock = 0;
}

/**
 * t_t1xhv_status status, t_t1xhv_encoder_info info
 */
void METH(endCodec)(t_uint32 sts, t_uint32 info, t_uint32 duration)
{
	void *output;
    int output_buffer_size;
	t_uint32 out_offset = 0;
	t_uint32 out_size;
	t_uint32 stuffing_bytes;
	t_uint32 len_sei = 0;
	t_hamac_param* h_param;

	if (sts == 100)
	{
        OstTraceFiltInst1(TRACE_FLOW, "Signal caught %d", info);
		return;
	}

    OstTraceFiltInst1(TRACE_FLOW, "end codec called lock = %d", lock);
	lock = 1;

	h_param = &cmd_fifo[idx_fifo_in_pull];
	output = h_param->output_buffer;
    output_buffer_size = dec_static.host.buff->bs_size;

#ifdef _CACHE_OPT_
    VFM_CacheInvalidate(vfm_memory_ctxt, (t_uint8*)h_param->output_buffer, output_buffer_size);
#endif

    OstTraceFiltInst1(TRACE_FLOW, "endCodec called for frame %d", h_param->picture_number + 1);

#ifndef __ndk5500_a0__
	if (sts == STATUS_JOB_ABORTED) {
        OstTraceFiltInst0(TRACE_FLOW, "Encode cancelled");
		//return;
	}
#endif

#ifndef __ndk5500_a0__
	dump_struct("vec_h264_param_inout", h_param->picture_number, h_param->addr_out_frame_parameters, sizeof(*h_param->addr_out_frame_parameters));
#endif
	dump_struct("vec_h264_param_out", h_param->picture_number, h_param->addr_out_parameters, sizeof(*h_param->addr_out_parameters));

#ifdef __ndk5500_a0__
	switch ((t_hva_h264enc_status)sts)
	{
		case H264ENC_ENCODE_OK:
			break;
		case H264ENC_ABORT_PIC_OVER_BITSREAM_SIZE:
			break;
		case H264ENC_ABORT_MB_OVER_SLICE_SIZE:
			break;
		case H264ENC_FRAME_SKIPPED:
			skip_current = 1;
			break;
	};
#else
	skip_current = h_param->addr_out_frame_parameters->Skip_Current;
	skip_next = h_param->addr_out_frame_parameters->Skip_Next;
#endif

#ifdef __ndk5500_a0__
	/*remove this*/
	t_uint8 *ptr;
        int j;
	ts_t1xhv_vec_h264_param_inout *param_inout =
		(ts_t1xhv_vec_h264_param_inout*)VFM_GetLogical(vfm_memory_ctxt, (t_uint8 *)h_param->addr_in_parameters->addr_param_inout);
	for(j = 0; j < sizeof(*h_param->addr_out_frame_parameters); j++)
	{
		((t_uint8*)h_param->addr_out_frame_parameters)[j] = ((t_uint8*)param_inout)[j];
	}
#endif

#ifdef __ndk5500_a0__
	out_size = h_param->addr_out_frame_parameters->bitstream_size + h_param->header_size;
#else
	out_size = Endianess(h_param->addr_out_frame_parameters->bitstream_size) / 8 + h_param->header_size;
#endif

    OstTraceFiltInst1(TRACE_FLOW, "bs size = %d", out_size);

	if (sei_present)
	{
		pic_parameter_set_rbsp_t active_pps;
		seq_parameter_set_rbsp_t active_sps;
		char buffer[4]= {0x00, 0x00, 0x00, 0x01};

		Gsp_sps = &active_sps;
		FillParameterSetStructures(&active_sps, &active_pps, h_param->info.EncoderLevel, &h_param->info, h_param->addr_in_parameters);

		memcpy(((t_uint8*)output) + h_param->header_size, buffer, 4);

		bitbuf = ((t_uint8*)output) + h_param->header_size;
		bitbuf += 4;
		{
			ts_NALU nalu_s;
			tps_NALU nalu = &nalu_s;



			Gsp_img = h_param->addr_in_parameters;
			Gsp_inout = h_param->addr_in_frame_parameters;

			InitSEIio(SEIio);

			SEIio->NALAUsize = Endianess(Gsp_img->NonVCLNALUSize); // + /*Gs_hamac_task_info.addr_out_frame_parameters.*/Gsp_inout->bitstream_size);
			SEIio->VCLAUsize = 0; ///*Gs_hamac_task_info.addr_out_frame_parameters.*/Gsp_inout->bitstream_size;

			/* NZ: now copy the removal timestamp from hamac to SEI struct */
			//SEIio->currAUts = /*Gs_hamac_task_info.ddr_out_frame_parameters.*/2;//Gsp_inout->removal_time;
			SEIio->currAUts = Endianess(h_param->addr_out_frame_parameters->removal_time);
			SEIio->lastBPAUts = Endianess(Gsp_inout->lastBPAUts);

			/* set the flag InitBuffer for first input image */
			SEIio->InitBuffer = /*(Gsp_img->number==0)*/Gsp_img->FirstPictureInSequence;
			//if (Gsp_img->FirstPictureInSequence)
			//	lastBPAUts = Endianess(h_param->addr_out_frame_parameters->removal_time);
			/*if (!SEIio->InitBuffer)
				SEIio->currAUts = h_param->picture_number + 2;*/

			/* compute and write SEI messages in the stream */
			if (!skip_current)
			{
				/* NZ: rbsp buffer statically alloc'ed in host_nalucommon.h */
				nalu->buf = bitbuf;

				GenerateSEI_NALU(nalu);        /* generate SEI NAL */

				len_sei = 4 + nalu->len;

				sei_message_payloadSize = 0; /* reset payload size (ready for next SEI NALU) */
				nalu = (tps_NALU)NULL;
			}

            OstTraceFiltInst2(TRACE_FLOW, "SEI size %d for frame %d", len_sei, h_param->picture_number + 1);
			NonVCLNALUSize -= len_sei;
		}
		UpdateFinalArrivalTime(SEIio);
	}

	if (0) {
        OstTraceFiltInst3(TRACE_FLOW, "COPY... bs_size:%d header_size:%d len_sei:%d", out_size, h_param->header_size, len_sei);
		t_uint32 j;
		//TODO ndk5500_a0 case -> s already includes header_size
		for(j = 0; j < (out_size - h_param->header_size); j++)
		{
			(((t_uint8*)output) + h_param->header_size + len_sei)[j] = (((t_uint8*)output) + h_param->header_size + MAX_SEI_SIZE)[j];
		}
        OstTraceFiltInst0(TRACE_FLOW, "");
        OstTraceFiltInst0(TRACE_FLOW, "END COPY...");
	}

	if (sei_present) {
        OstTraceFiltInst3(TRACE_FLOW, "COPY... bs_size:%d header_size:%d len_sei:%d", out_size, h_param->header_size, len_sei);
		t_uint32 j;
		//TODO ndk5500_a0 case -> s already includes header_size
		for(j = 0; j < (h_param->header_size + len_sei); j++)
		{
			((t_uint8*)output)[h_param->header_size + MAX_SEI_SIZE - 1 - j] = ((t_uint8*)output)[h_param->header_size + len_sei - 1 - j];
		}
        OstTraceFiltInst0(TRACE_FLOW, "");
        OstTraceFiltInst0(TRACE_FLOW, "END COPY...");
		out_offset = MAX_SEI_SIZE - len_sei;
	}

	out_size += len_sei;

	stuffing_bytes = Endianess(h_param->addr_out_frame_parameters->stuffing_bits) / 8;
    OstTraceFiltInst1(TRACE_FLOW, "H264enc END ALGO, STUFFING BYTES sb = %d", stuffing_bytes);
	if (stuffing_bytes) {
		if (out_size + 6 + stuffing_bytes > dec_static.buf.bs_size)
        {
            OstTraceFiltInst2(TRACE_FLOW, "size %d > %d", out_size + 6 + stuffing_bytes, dec_static.buf.bs_size);
        }
		else {
			t_uint8* mybuf = ((t_uint8*)output)+out_size;

			mybuf[0] = 0;
			mybuf[1] = 0;
			mybuf[2] = 0;
			mybuf[3] = 1;
			mybuf[4] = 0x0c;
			memset(mybuf+5, 0xff, stuffing_bytes);
			mybuf[5+stuffing_bytes] = 0x80;
			out_size += 6 + stuffing_bytes;
		}
	}

	if (OutFileMode) {
		t_uint32 i;
		t_uint8* p_slice = (t_uint8*)0;
		t_uint8* mybuf = (t_uint8*)output;
		mybuf += h_param->header_size;
		int sz = -4;
		for (i = 0; i < (out_size-h_param->header_size-4); i++)
		{
			//printf(">> %d / %d\n", i, s-h_param->header_size);
			if (!mybuf[i] && !mybuf[i+1] && !mybuf[i+2] && (mybuf[i+3] == 1))
			{
				//printf("## %d\n", i);
				if (p_slice)
				{
					p_slice[0] = (sz >> 24) & 0xFF;
					p_slice[1] = (sz >> 16) & 0xFF;
					p_slice[2] = (sz >>  8) & 0xFF;
					p_slice[3] = (sz >>  0) & 0xFF;
				}
				sz = -4;
				p_slice = &mybuf[i];
			}
			sz++;
		}
		sz += 4;
		p_slice[0] = (sz >> 24) & 0xFF;
		p_slice[1] = (sz >> 16) & 0xFF;
		p_slice[2] = (sz >>  8) & 0xFF;
		p_slice[3] = (sz >>  0) & 0xFF;
	}

    t_valid_output valid_out = VALID_OUT_OTHER;
    if (skip_current)
    {
        valid_out = VALID_OUT_OTHER;
    }
    else if (h_param->addr_in_parameters->picture_coding_type == I_SLICE)
    {
        valid_out = VALID_OUT_I_FRAME;
    }
    else
    {
        valid_out = VALID_OUT_P_FRAME;
    }


	notify_output_buffer.notify_output_buffer(output, out_offset, out_size, valid_out, 0, 0, h_param->flags);
	notify_input_buffer.notify_input_buffer(h_param->p_input_buffer, h_param->in_size, VALID_OUT_OTHER, 0, 0, 0);

#ifdef __ndk5500_a0__
	{
		ts_t1xhv_vec_h264_param_inout* tmp = hamac_param.addr_in_frame_parameters;
		hamac_param.addr_in_frame_parameters = hamac_param.addr_out_frame_parameters;
		hamac_param.addr_out_frame_parameters = tmp;
	}
#endif
	OstTraceFiltInst2(TRACE_FLOW, "current = %d, next = %d", skip_current, skip_next);
	idx_fifo_in_pull = (idx_fifo_in_pull + 1) % CMD_FIFO_SIZE;
	//printf("idx_fifo = %d\n", idx_fifo);
	//idx_fifo_out = (idx_fifo_out + 1) % CMD_FIFO_SIZE;

	if (skip_current) {
		frame_number = h_param->frame_number;
		FramePictureCounter = h_param->FramePictureCounter;
		frame = h_param->frame;
	}

	if (skip_current)
	{
		/* Requeue commands */
		t_hamac_param* h_p;
		int i = idx_fifo_in_pull;
		while (i != idx_fifo_in_push)
		{
			t_uint32 tmp;
			h_p = &cmd_fifo[i];
			tmp = h_p->addr_out_frame_buffer->addr_dest_buffer;
			h_p->addr_out_frame_buffer->addr_dest_buffer = h_p->addr_in_frame_buffer->addr_fwd_ref_buffer;
			h_p->addr_in_frame_buffer->addr_fwd_ref_buffer = tmp;

            OstTraceFiltInst1(TRACE_FLOW, "reissue frame %d", h_p->picture_number + 1);

			if (frame == 16)
				frame = 0;

			if ((frame_number % h_p->IntraPeriod) == 0) {
				h_p->addr_in_parameters->idr_flag = h_p->IDRIntraEnable;
				if (h_p->IDRIntraEnable == 1) {
					frame = 0;
				}
			}
			else {
				h_p->addr_in_parameters->idr_flag = 0;
			}

			h_p->addr_in_parameters->frame_num = Endianess(frame);

			if ((frame_number % h_p->IntraPeriod) == 0) {
				h_p->addr_in_parameters->picture_coding_type = I_SLICE;
			} else {
				h_p->addr_in_parameters->picture_coding_type = P_SLICE;
			}

			h_p->frame_number = frame_number;
			h_p->FramePictureCounter = FramePictureCounter;
			h_p->frame = frame;

			frame_number++;
			FramePictureCounter++;
			frame++;
			dump_struct("vec_h264_param_in", h_p->picture_number, h_p->addr_in_parameters, sizeof(*h_p->addr_in_parameters));
#ifndef __ndk5500_a0__
			dump_struct("vec_h264_param_inout", h_p->picture_number, h_p->addr_in_frame_parameters, sizeof(*h_p->addr_in_frame_parameters));
#endif
			call_dsp(vfm_memory_ctxt, h_p);
			i = (i + 1) % CMD_FIFO_SIZE;
		}
	}

	lock = 0;
}


void METH(initTraceInfo)(TraceInfo_t * ptr, unsigned int val)
{
    OstTraceInt0(TRACE_FLOW, "[ENCODER] > initTraceInfo()");
    setTraceInfo(ptr,val);
#ifdef __ndk5500_a0__
    traceInitAck();
    //mpc_trace_init.traceInit((t_uint16)((ptr >> 16) & 0xFFFF), (t_uint16)(ptr & 0xFFFF), val);
#else
    mpc_trace_init.traceInit((t_uint16)((ptr->dspAddr >> 16) & 0xFFFF), (t_uint16)(ptr->dspAddr & 0xFFFF), val);
#endif
    OstTraceInt0(TRACE_FLOW, "[ENCODER] < initTraceInfo()");
}


void METH(traceInitAck)()
{
    pendingCommandAck.pendingCommandAck(0);
}



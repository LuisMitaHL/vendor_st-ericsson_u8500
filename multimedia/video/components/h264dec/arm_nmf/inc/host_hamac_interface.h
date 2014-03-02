/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_HAMAC_INTERFACE_H
#define HOST_HAMAC_INTERFACE_H

#include <t1xhv_common.idt>
#include <t1xhv_vdc_h264.idt>

typedef struct hed_param
{
  t_uint32 HED_used;
  t_uint32 bitstream_start;
  t_uint32 bitstream_end;
  t_uint32 ib_start;
  t_uint32 hed_cfg;
  t_uint32 hed_picwidth;
  t_uint32 hed_codelength;
} t_hed_param;

typedef struct hamac_param
{
	// needed to help address translation and alocation for each set of param in 
	t_uint32	offset_addr;
	t_uint32	heap_size;
	t_uint8		*heap_addr;
	t_uint8		*heap_current_addr;

	// use by DSP
	ts_t1xhv_vdc_frame_buf_in*	 	 addr_in_frame_buffer;                            
	ts_t1xhv_vdc_frame_buf_out*	 	 addr_out_frame_buffer;                            
	ts_t1xhv_vdc_internal_buf*	 	 addr_internal_buffer;                            
	ts_t1xhv_bitstream_buf_pos*	 	 addr_in_bitstream_buffer;                            
	ts_t1xhv_bitstream_buf_pos*	 	 addr_out_bitstream_buffer;                            
	ts_t1xhv_vdc_h264_param_in*	 	 addr_in_parameters;
	ts_t1xhv_vdc_h264_param_out*	 addr_out_parameters;
	void*						 	 addr_in_frame_parameters;
	void*				 			 addr_out_frame_parameters;
  t_hed_param hed_params;
}
t_hamac_param;


void HamacDeallocSlice(t_hamac_param *t,t_uint16 n);
void HamacAllocSlice(t_hamac_param *t, t_uint16 n);

#if 0
void HamacFillParamIn(void *vfm_memory_context,t_dec *dec, t_uint16 nslices,t_hamac_param *t);
void HamacAllocParameter(void *vfm_memory_context,t_hamac_param *t,t_uint16 n);
void HamacDeallocParameter(void *vfm_memory_context,t_hamac_param *t, t_uint16 n);
void HamacToPhysicalAddresses(void *vfm_memory_context,t_hamac_param *t, t_uint16 nslices);
void HamacToLogicalAddresses(void *vfm_memory_context,t_hamac_param *t, t_uint16 nslices);
#endif
#endif //HOST_HAMAC_INTERFACE_H

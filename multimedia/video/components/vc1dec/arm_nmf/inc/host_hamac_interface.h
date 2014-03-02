/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_HAMAC_INTERFACE_H
#define HOST_HAMAC_INTERFACE_H

#include <t1xhv_common.idt>
#include <t1xhv_vdc_vc1.idt>

typedef struct hamac_param
{
	ts_t1xhv_vdc_frame_buf_in*	 	 addr_in_frame_buffer;                            
	ts_t1xhv_vdc_frame_buf_out*	 	 addr_out_frame_buffer;                            
	ts_t1xhv_vdc_internal_buf*	 	 addr_internal_buffer;                            
	ts_t1xhv_bitstream_buf_pos*	 	 addr_in_bitstream_buffer;                            
	ts_t1xhv_bitstream_buf_pos*	 	 addr_out_bitstream_buffer;                            
	ts_t1xhv_vdc_vc1_param_in*	 	 addr_in_parameters;
	ts_t1xhv_vdc_vc1_param_out*	 addr_out_parameters;
	void*						 	 addr_in_frame_parameters;
	void*				 			 addr_out_frame_parameters;
}
t_hamac_param;

void HamacFillParamIn(void *local_vfmmemory_ctxt,struct t_dec_type *dec, t_uint16 nslices,t_hamac_param	*t);
void HamacFillParameterIn(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_vc1_param_in_common *p,ts_sequence_parameters *seq_params,t_uint16 pic_width,t_uint16 pic_height);
void HamacAllocParameter(void *vfm_memory_context,t_hamac_param *t,t_uint16 n);
void HamacAllocParameterIn(void *vfm_memory_context,ts_t1xhv_vdc_vc1_param_in_common **p);
void HamacDeallocParameter(void *vfm_memory_context,t_hamac_param *t, t_uint16 n);
void HamacDeallocParameterIn(void *vfm_memory_context,ts_t1xhv_vdc_vc1_param_in_common *p);
void HamacToPhysicalAddresses(void *vfm_memory_context,t_hamac_param *t, t_uint16 nslices);
void HamacToPhysicalAddresses(void *vfm_memory_context,ts_t1xhv_vdc_vc1_param_in_common **p);
void HamacToLogicalAddresses(void *vfm_memory_context,t_hamac_param *t, t_uint16 nslices);

#endif //HOST_HAMAC_INTERFACE_H

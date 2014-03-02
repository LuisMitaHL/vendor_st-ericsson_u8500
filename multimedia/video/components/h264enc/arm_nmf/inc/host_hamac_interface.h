/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_HAMAC_INTERFACE_H
#define HOST_HAMAC_INTERFACE_H

#include <t1xhv_common.idt>
#include <t1xhv_vec_h264.idt>
#include <../api/specific_frameinfo.idt>

typedef struct hamac_param
{
    ts_t1xhv_vec_frame_buf_in*     addr_in_frame_buffer;                            
    ts_t1xhv_vec_frame_buf_out*    addr_out_frame_buffer;                            
    ts_t1xhv_vec_internal_buf*     addr_internal_buffer;                            
    ts_t1xhv_bitstream_buf_pos*    addr_in_bitstream_buffer;                            
    ts_t1xhv_bitstream_buf_pos*    addr_out_bitstream_buffer;                            
    ts_t1xhv_vec_h264_param_in*    addr_in_parameters;
    ts_t1xhv_vec_h264_param_out*   addr_out_parameters;
    ts_t1xhv_vec_h264_param_inout* addr_in_frame_parameters;
    ts_t1xhv_vec_h264_param_inout* addr_out_frame_parameters;
    ts_t1xhv_bitstream_buf*        addr_bitstream_buf_struct;
    void *p_input_buffer;
    t_uint32 in_size;
    t_uint32 flags;
    void* output_buffer;
    t_uint32 frame_number;
    t_uint32 FramePictureCounter;
    t_uint32 picture_number;
    t_uint32 frame;
    t_uint32 header_size;
//+ code for step 2 of CR 332873
    t_uint32 config_change;
//- code for step 2 of CR 332873
    t_uint16 IntraPeriod;
    t_uint16 IDRIntraEnable;
    t_specific_frameinfo info;
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

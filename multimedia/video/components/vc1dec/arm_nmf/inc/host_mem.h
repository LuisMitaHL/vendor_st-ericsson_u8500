/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_MEM_H
#define HOST_MEM_H
/*
#include "types.h"
#include "host_types.h"
#include "settings.h"
#include "host_hamac_pipe.h"
#include "host_decoder.h"
*/
t_bit_buffer *AllocateBitstreamBuffer(void *vfm_memory_ctxt,t_uint32 bytes);
//void AllocateSliceArray(void *vfm_memory_ctxt,t_slice_hdr **sha);
//void ReallocateSliceArray(void *vfm_memory_ctxt,t_uint16 *maxslices, t_slice_hdr **sha);
//void AllocateMemory(void *vfm_memory_ctxt,t_seq_par *p_sp, t_dec_buff *p_buff,t_hamac_pipe *hamac_pipe,t_input_control *input_control);
//void FreeMemory(void *vfm_memory_ctxt,t_dec_buff *p_buff,t_hamac_pipe *hamac_pipe);
void FreeBitstreamBuffer(void *vfm_memory_ctxt,t_bit_buffer **p_b);
//void FreeSliceMemory(void *vfm_memory_ctxt,t_slice_hdr *sha);
//void HamacDeallocSlice(void *vfm_memory_ctxt,t_hamac_param *t,t_uint16 n);
//void HamacAllocSlice(void *vfm_memory_ctxt,t_hamac_param *t, t_uint16 n);


#endif

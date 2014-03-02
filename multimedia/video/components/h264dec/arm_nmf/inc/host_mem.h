/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_MEM_H
#define HOST_MEM_H

#include "types.h"
#include "host_types.h"
#include "settings.h"
#include "host_decoder.h"
#include <omx_define.h>

#include "OMX_Component.h"
#include "OMX_Types.h"


t_bit_buffer		 	*AllocateBitstreamBuffer(void *vfm_memory_ctxt,t_uint32 bytes);
OMX_BUFFERHEADERTYPE	*AllocateOmxBitstreamBuffer(void *vfm_memory_ctxt,t_uint32 bytes);

t_uint16 AllocateSliceArray(void *vfm_memory_ctxt,t_slice_hdr **sha);
t_uint16 ReallocateSliceArray(void *vfm_memory_ctxt,t_uint16 *maxslices, t_slice_hdr **sha);
//void AllocateMemory(void *vfm_memory_ctxt,t_seq_par *p_sp, t_dec_buff *p_buff);
//void FreeMemory(void *vfm_memory_ctxt,t_dec_buff *p_buff);

void FreeBitstreamBuffer(void *vfm_memory_ctxt,t_bit_buffer *p);
void FreeOmxBitstreamBuffer(void *vfm_memory_ctxt,OMX_BUFFERHEADERTYPE *p);
void FreeSliceMemory(void *vfm_memory_ctxt,t_slice_hdr *sha);
void HamacDeallocSlice(void *vfm_memory_ctxt,t_hamac_param *t,t_uint16 n);
void HamacAllocSlice(void *vfm_memory_ctxt,t_hamac_param *t, t_uint16 n);


#endif

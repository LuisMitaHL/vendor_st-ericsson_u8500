/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_BLOCK_INFO_H
#define HOST_BLOCK_INFO_H

#include "types.h"
#include "host_types.h"

void resetBlockInfo(t_seq_par *p_sp, t_dec_buff *p_buff);
t_uint16 NotDecoded(t_uint16 mbaddr, t_block_info *p_b_info);

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
t_uint16 AllocCupContextArray(void *vfm_memory_ctxt,t_uint32 mbx,t_uint32 mby, t_dec_buff *p_buff);
void FreeCupContextArray(void *vfm_memory_ctxt,t_dec_buff *p_buff);
void InitCupContextFifo(t_dec_buff *p_buff);

inline void cup_context_fifo_reset(t_cup_context_descriptor* desc) {
  t_uint16 ii;
	desc->cup_ctx_avail_fifo_r = desc->cup_ctx_avail_fifo_w = 0;
	for(ii=0;ii<CUP_CTX_AVAIL_FIFO_SIZE;ii++)
	 desc->cup_ctx_avail_fifo[ii] = -1;
}

inline t_sint16 cup_context_fifo_push(t_cup_context_descriptor* desc, t_sint16 index){
	t_uint16 pos, new_pos;
	
	if(index==-1)return 0; // in case a dummy index is pushed, we simply ignore it

	new_pos = desc->cup_ctx_avail_fifo_w + 1;
	if (new_pos >= CUP_CTX_AVAIL_FIFO_SIZE) new_pos=0;
	if (new_pos == desc->cup_ctx_avail_fifo_r) return -1; // FIFO full
	pos = desc->cup_ctx_avail_fifo_w;
	desc->cup_ctx_avail_fifo_w = new_pos;
	desc->cup_ctx_avail_fifo[pos] = index;
#if VERBOSE_CUP_FIFO == 1
  NMF_LOG("CUP-CTX-FIFO : Pushed index %d\n", index);
#endif
	return 0;
}

inline t_sint16 cup_context_fifo_pop(t_cup_context_descriptor* desc) {
  t_sint16 index = desc->cup_ctx_avail_fifo[desc->cup_ctx_avail_fifo_r];
	if(desc->cup_ctx_avail_fifo_r == desc->cup_ctx_avail_fifo_w) index = -1; // FIFO empty
	desc->cup_ctx_avail_fifo_r++;
	if (desc->cup_ctx_avail_fifo_r >= CUP_CTX_AVAIL_FIFO_SIZE) desc->cup_ctx_avail_fifo_r=0;
#if VERBOSE_CUP_FIFO == 1
  NMF_LOG("CUP-CTX-FIFO : Poped index %d\n", index);
#endif
	return index;
}

#endif
		
#endif

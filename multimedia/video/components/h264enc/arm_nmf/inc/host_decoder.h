/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_DEC_H
#define HOST_DEC_H

#include "types.h"
#include "host_types.h"
#include "host_hamac_pipe.h"

#define CMD_FIFO_SIZE 10

#if 0
typedef struct t_input_control_type
{
	t_sint16 dpb_frame_available;
	t_uint16 pending_ack;
	t_bit_buffer *bit_buffer_list;
}
t_input_control;
#endif

typedef struct t_dec_type
{
#if 0
 	t_uint16 	sh_status;
	t_uint16	nut;
	t_uint16	stop;
	t_sint16 	num_seq_par;
	t_sint16 	num_pic_par;
	t_uint16 	next_idr;		/* next expected slice must be an IDR         */
	t_uint16 	failure; 		/* memorised if a problem has been encountered */
    t_uint16 	need_resync;
    t_uint16 	first_slice;
    t_uint16 	sps_active ;
    t_uint16 	error_count;
    t_uint16 	use_rs;
    
 
	t_uint16	maxslices;
    t_sint16 	slice_num;
	t_seq_par stab[32];
    t_pic_par ptab[256];
    t_sint32 	nframes;
    t_slice_hdr *prevSh;
    t_slice_hdr old_sh;
	t_seq_par 	*p_active_sp;
    t_seq_par 	old_sp;
    t_pic_par 	*p_active_pp;
    t_slice_hdr *sh;
    t_old_slice old_sl;
#endif
    t_host_info host;
    t_dec_buff  buf;

	//t_input_control input_control;
	t_hamac_pipe hamac_pipe;
}
t_dec;


void	init_decoder(void);
void 	close_decoder(void);

#ifdef __cplusplus
extern "C" {
#endif
void HamacFillParamIn(void *vfm_memory_context,t_dec *dec, t_uint16 nslices, t_hamac_param *t);
void HamacAllocParameter(void *vfm_memory_context,t_hamac_param *t);
void HamacDeallocParameter(void *vfm_memory_context,t_hamac_param *t);
void HamacToPhysicalAddresses(void *vfm_memory_context,t_hamac_param *t);
void HamacToLogicalAddresses(void *vfm_memory_context,t_hamac_param *t);
#ifdef __cplusplus
}
#endif

#endif

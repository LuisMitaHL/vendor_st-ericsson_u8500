/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __fileReader_cpp
#define __fileReader_cpp

#include "h264enc/arm_nmf/inc/settings.h"
#include "h264enc/arm_nmf/inc/types.h"
#include "h264enc/arm_nmf/inc/host_types.h"
#include "h264enc/arm_nmf/inc/hamac_types.h"
#include "h264enc/arm_nmf/inc/host_decoder.h"
//#include "h264enc/arm_nmf/inc/host_sei.h"
#include "h264enc/arm_nmf/encoder/src/h264enc_host_sei.h"

#include "h264enc/arm_nmf/api/nmftype.idt"
#include "VFM_Memory.h"
#include "TraceObject.h"

class h264enc_arm_nmf_encoder: public h264enc_arm_nmf_encoderTemplate, public TraceObject
{
/* methods itf */
public:
virtual t_nmf_error construct(void);
virtual void        destroy(void);

// Interfaces
virtual void init(void *mem_ctxt);
virtual void close(void);
virtual void input(void *p_sp, t_frameinfo v_params, t_uint32 nFlags);
virtual void get_headers(t_frameinfo* p_params, void* buffer1, t_uint16* size1, void* buffer2, t_uint16* size2, t_uint16 no_headers_stream);
//virtual void endCodec(t_t1xhv_status a, t_t1xhv_encoder_info b, t_uint32 duration);
virtual void endCodec(t_uint32 a, t_uint32 b, t_uint32 duration);
virtual void output_buffer(void *ptr,t_uint32 size16);
// Internal function due to NMF C++ framework
void call_dsp(void *mem_ctxt, t_hamac_param *h_param);
void CirInit(t_uint16 seed, t_host_info *host, t_uint16 CirMbNum,t_uint16 IntraType);


    //Trace
    virtual void initTraceInfo(TraceInfo_t * ptr, unsigned int val);
    virtual void traceInitAck();
private:
void dump_struct(const char* name, t_uint32 pic, void* ptr_, t_uint32 size);
void dump_params(t_hamac_param *h_param);
void* AllocESRAM(void *vfm_memory_ctxt,int size, int align, bool inside_one_bank);
void AllocateMemory(void *vfm_memory_ctxt, t_frameinfo *p_params, t_host_info *host, t_hamac_pipe *hamac_pipe);
public:
t_dec 	dec_static;
void	*vfm_memory_ctxt;
int     buff_inited;
int     OutFileMode;
t_hamac_param hamac_param;
t_uint32 header_size;
t_uint32 picture_number;
t_uint32 frame_number;
t_uint32 FramePictureCounter;
t_uint32 frame;
t_uint32 lastBPAUts;
t_uint16 no_headers;
t_uint16 sps_id;
//+ code for step 2 of CR 332873
t_uint32 last_bitrate;
t_uint32 last_framerate;
//- code for step 2 of CR 332873
volatile int async_init_done; /* patch by MC to make sure that interfaces are called in-order */
t_uint16 sei_present;
t_uint32 NonVCLNALUSize;
t_uint16 skip_current, skip_next;
t_uint16 mv_toggle;
void *p_output_buffer[CMD_FIFO_SIZE];
t_uint32 output_size[CMD_FIFO_SIZE];
int idx_fifo_out_push;
int idx_fifo_out_pull;
t_hamac_param cmd_fifo[CMD_FIFO_SIZE];
int idx_fifo_in_push;
int idx_fifo_in_pull;
int lock;
t_uint32 last_theorical_timestamp;
t_uint32 timestamp;
};

#endif


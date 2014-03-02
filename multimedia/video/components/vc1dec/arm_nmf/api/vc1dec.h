/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _vc1dec_h
#define _vc1dec_h

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
void vc1dec_arm_nmf_picture(void *p_sp, t_uint16 **p_fb);
void vc1dec_arm_nmf_splitter_bitstream(t_bit_buffer	*p_b);

void vc1dec_arm_nmf_vc1dec_construct(void);
void vc1dec_arm_nmf_vc1dec_destroy(void);
void vc1dec_arm_nmf_vc1dec_start(void);
void vc1dec_arm_nmf_vc1dec_stop(void);

void vc1dec_arm_nmf_vc1dec_sw_construct(void);
void vc1dec_arm_nmf_vc1dec_sw_destroy(void);
void vc1dec_arm_nmf_vc1dec_sw_start(void);
void vc1dec_arm_nmf_vc1dec_sw_stop(void);

void vc1dec_arm_nmf_vc1dec_pipe_construct(void);
void vc1dec_arm_nmf_vc1dec_pipe_destroy(void);
void vc1dec_arm_nmf_vc1dec_pipe_start(void);
void vc1dec_arm_nmf_vc1dec_pipe_stop(void);


#ifdef __cplusplus
}
#endif

#endif

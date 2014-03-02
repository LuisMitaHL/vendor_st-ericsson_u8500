/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _h264enc_h
#define _h264enc_h

#ifdef __cplusplus
extern "C" {
#endif

//#include <types.h>

void h264enc_arm_nmf_h264enc_construct(void);
void h264enc_arm_nmf_h264enc_destroy(void);
void h264enc_arm_nmf_h264enc_start(void);
void h264enc_arm_nmf_h264enc_stop(void);

void h264enc_arm_nmf_h264enc_stub_construct(void);
void h264enc_arm_nmf_h264enc_stub_destroy(void);
void h264enc_arm_nmf_h264enc_stub_start(void);
void h264enc_arm_nmf_h264enc_stub_stop(void);


#ifdef __cplusplus
}
#endif

#endif

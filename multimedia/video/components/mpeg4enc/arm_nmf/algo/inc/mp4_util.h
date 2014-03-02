/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

int  mp4e_FindBlockEvents(short *block, int inter, int LastCoeff);
#ifdef __ARM_NEON__
int  mp4e_FindBlockEvents_NEON(short *block, int inter);
#endif
void mp4e_EraseBlock(void *p, int Bytesize);
void mp4e_rasterize_block(unsigned char *position_in, short *block, int x_dim);
void mp4e_rasterize_block_chroma(unsigned char *position_in, short *block, int x_dim, int V);
void mp4e_DifferenceBlock(unsigned char * RESTRICT in1, unsigned char * RESTRICT in2, short * RESTRICT out, int xdim, int vop_width_actual);
void mp4e_DifferenceBlock_Chroma(unsigned char * RESTRICT in1, unsigned char * RESTRICT in2, short * RESTRICT out, int xdim, int V, int vop_width_actual);
int  mp4e_sign(int x);
t_uint16 mp4e_Qp_SQRT(t_sint32 val);

#endif /* UTIL_H */

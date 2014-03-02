/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef UTIL_NEON_H
#define UTIL_NEON_H

int mp4e_BlockSAE_NEON (short *p);
int mp4e_FindBlockEvents_NEON(short *block, int inter);
void mp4e_DifferenceBlock_NEON(unsigned char * RESTRICT in1, unsigned char * RESTRICT in2, short * RESTRICT out, int xdim);
void mp4e_rasterize_block_NEON(unsigned char *position_in, short *block, int x_dim);
void mp4e_EraseBlock_NEON(void *p);
#endif

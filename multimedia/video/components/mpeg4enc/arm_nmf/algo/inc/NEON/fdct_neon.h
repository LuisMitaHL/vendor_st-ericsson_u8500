/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef FDCT_NEON_H
#define FDCT_NEON_H


/* DCT and IDCT */

void mp4e_fdct_intra_luma_NEON (unsigned char* raw_yuv, unsigned int width_actual, short *coeff);  //ip, op
void mp4e_fdct_inter_luma_NEON (unsigned char* raw_yuv, unsigned char* recon_yuv, unsigned int width_actual, unsigned int crop_width, short *coeff);  //ip, op
void mp4e_fdct_inter_chroma_NEON (unsigned char* raw_yuv, unsigned char* recon_yuv, unsigned int width_actual, unsigned int crop_width, short *coeff, int V);  //ip, op
void mp4e_fdct_intra_chroma_NEON (unsigned char* raw_yuv, unsigned int width_actual, short *coeff, int V);  //ip, op

#endif

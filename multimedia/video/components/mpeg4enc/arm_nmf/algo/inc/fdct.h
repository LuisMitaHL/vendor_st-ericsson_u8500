/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef FDCT_H
#define FDCT_H


/* DCT and IDCT */
#ifndef __ARM_NEON_FDCT__
void mp4e_fdct_intra_luma   (ui8_t* raw_yuv, ui32_t width_actual, si16_t *F);
void mp4e_fdct_inter_luma   (ui8_t* raw_yuv, ui8_t* recon_yuv, ui32_t width_actual, ui32_t crop_width, si16_t *F);
void mp4e_fdct_inter_chroma (ui8_t* raw_yuv, ui8_t* recon_yuv, ui32_t width_actual, ui32_t crop_width, si16_t *F, si32_t V);
void mp4e_fdct_intra_chroma (ui8_t* raw_yuv, ui32_t width_actual, si16_t *F, si32_t V);
#else //__ARM_NEON_FDCT__
void mp4e_fdct_intra_luma_NEON (unsigned char* raw_yuv, unsigned int width_actual, short *coeff);  //ip, op
void mp4e_fdct_inter_luma_NEON (unsigned char* raw_yuv, unsigned char* recon_yuv, unsigned int width_actual, unsigned int crop_width, short *coeff);  //ip, op
void mp4e_fdct_inter_chroma_NEON (unsigned char* raw_yuv, unsigned char* recon_yuv, unsigned int width_actual, unsigned int crop_width, short *coeff, int V);  //ip, op
void mp4e_fdct_intra_chroma_NEON (unsigned char* raw_yuv, unsigned int width_actual, short *coeff, int V);  //ip, op
#endif //__ARM_NEON_FDCT__

#endif

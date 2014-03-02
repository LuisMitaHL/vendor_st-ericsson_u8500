/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef DC_PREDICTION_H
#define DC_PREDICTION_H

#define PAL_DIM   80 /*HDTV Support 45*/
#define CIF_DIM   22
#define QCIF_DIM  11
#define MAX_DIM   PAL_DIM
#define DC_HALF   1024

#define NO_ACDC_PRED	-1
#define DC_ONLY		0
#define ACDC_ABOVE	1
#define ACDC_LEFT	2
typedef struct
{
  int MBnum_x;
  int MBnum_y;
  int blk_num;
  int A;
  int E;
  int ch_U_A;
  int ch_U_B;
  int ch_V_A;
  int ch_V_B;
  int video_packet_mb;
  int prev_video_packet_mb;
  short luma_previous_DC_coeff[(2*MAX_DIM)+2];
  short chroma_previous_DC_coeff[(2*MAX_DIM)+2];
  
  short  DC_coef[6];

/*data duplicated for CBRSL: BEGIN*/
  short prea_luma_previous_DC_coeff[(2*MAX_DIM)+2];
  short prea_chroma_previous_DC_coeff[(2*MAX_DIM)+2];
  int   prea_E;
  int   prea_ch_U_A;
  int   prea_ch_V_A;
/*data duplication: END*/
}DC_Data;

void mp4e_INIT_DC_PRED(int image_width, int image_height);

void mp4e_Initialize_DC_prediction(void);

void mp4e_ACDC_Prediction(short *mb, int flag_close_video_packet, int x_pos, int y_pos, int size_mb, int QP, int blk_num);
void mp4e_DC_prediction_INTER(int flag_close_video_packet, int x_pos, int y_pos);
void mp4e_ACDCCreateCopy(int mode);

#ifdef H263_P3
t_sint16 mp4e_H263P3_DC_Prediction(t_sint16 *pos_DCT_Q_mb, t_sint32 flag_close_video_packet, t_sint32 x, t_sint32 y, t_sint32 squant, t_sint32 j, t_sint32 sh_gob_hdr, t_sint32 mod_quant_mode, mp4_parameters *mp4_par);
void mp4e_H263P3_ReconACDCPredBlock(t_sint32 j, t_sint32 mode, t_sint32 x, t_sint32 y, t_sint16 *block, t_sint16 shortheader, mp4_parameters *mp4_par);
#endif

#endif

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*MaSa_ratectrl.h*/
#ifndef MASA_TM5_RATECTRL
#define MASA_TM5_RATECTRL

#include "ast_types.h"
#include "ast_defines.h"

/* these are as in Hamac spec ! */
#define CONST_QP_NO_BUFF				0
#define FRAME_BASED_BRC					1
#define LOW_DELAY_CBR					2
#define LVR_VBR						3
#define CONST_QP_VBV_ANNEXG_BUFF		4
#define CONST_QP_VBV_ANNEXG_BUFF_BIS	5
#define CONST_QP_VBV_HRD_BUFF			6
#define CONST_QP_VBV_HRD_BUFF_BIS		7
/* the following are not defined in Hamac spec */
#define MMS_VBR					8
#define NON_STANDARD_CBR				9
#define VBV_ANNEXG_CBR					10

#define I_TYPE 0
#define P_TYPE 1

#define _CBR_ENHANCED_MB_CONTROL

//here 1
/*  HAMAC vars structure    */
typedef struct {
  t_uint16  picture_coding_type;  /*  picture coding type: Intra or Inter           */
  t_uint32  vp_bit_size;
  t_uint32  vp_size_max;

  /*  in-out vars       */
  t_uint32  stuffing_bits;

  t_uint32  PictQpSum;          /*  brc_stats_prev_12 */
  t_uint32  S_overhead;         /*  brc_stats_prev_13 */
  //t_uint16  gov_flag;       /*  brc_stats_prev_16 */ /* ADDED BY ENZO */
  t_sint32  buffer_fullness_fake_TS;  

  t_uint32  Tmin_UF;
  t_uint16  MBnum;
  t_uint16  MBcount;
  t_uint32  Smb;
  t_uint16  C, C1, Cmb, avgC;
  t_uint16  BPP, BPP1, BPPmb, avgBPP;
  t_sint32  B;
  t_uint16  Q, Qp, prevQp;
  t_uint32  HeaderCount;
  t_uint32  count;
  t_uint32  S_coeff;

#ifdef _CBR_ENHANCED_MB_CONTROL
  /* GT: enanched MB layer control for better Qp distribution, 02 Apr 07 */
  t_uint16 BPPmbPrevAct;
  t_sint32 BPPmbErr;
  t_uint16 BPPmbPrevEst;
  t_sint32 Lprev;
  t_uint16 CodedMB;
  t_uint16 CodedMB_flag;
#endif

/* segmented mode */
    t_uint16 avgPictQp;
    t_uint16 GOBmbsize;
    t_uint32 maxGOBbitsize;
    t_uint32 GOBQpsum;
    t_sint32 prevGOBcount;
    t_uint32 saved_PictQpSum;
    t_uint32 PictQpSumIntra;
    t_uint16 target_GOB_overflow;
    t_uint16 re_encode_MBskip;
    t_uint16 re_encode_GOB;  
} CBR_HAMAC_par;


void		mp4e_MaSaCBR_InitSeq(mp4_parameters * mp4_par);
t_sint16	mp4e_MaSaCBR_InitPict(void);
t_uint16    mp4e_MaSaCBR_MbMQuant(t_uint32 mb_type);
void		MaSaVBR_MB(t_sint32);
t_uint16    mp4e_MaSaCBR_PostPict(mp4_parameters *mp4_par);
void		mp4e_MaSa_OverHead(void);
t_uint16    mp4e_MaSa_UpdateBRCData(t_uint16 q,t_uint16 coded);
void		mp4e_MaSaCBR_PostSeq(void);
t_sint32	mp4e_StuffingMB(t_sint32,t_sint32,t_sint32,t_sint32,t_sint32);
void        mp4e_MaSa_recovery(void);

#endif /* MASA_TM5_RATECTRL */

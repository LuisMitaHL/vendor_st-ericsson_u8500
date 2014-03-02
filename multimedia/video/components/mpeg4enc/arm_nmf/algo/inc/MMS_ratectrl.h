/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*MMS_ratectrl.h
Header file of the TM5 VBR rate control*/
#ifndef MMS_RATECTRL
#define MMS_RATECTRL

#include "ast_types.h"
#include "ast_defines.h"

#define I_TYPE 0
#define P_TYPE 1

void mp4e_MMS_InitSeq(mp4_parameters * mp4_par);

t_sint32    mp4e_MMS_InitPict(void);
t_sint16	mp4e_MMS_MbMQuant(t_sint32 SAD);
void	    mp4e_MMS_PostSeq(void);
t_sint32	mp4e_VBR_StuffingMB(t_sint32 inter, t_sint32 used_bits, t_sint32 MB_index, t_sint32 VP_occupancy, t_sint32 VP_stuff);
t_uint16	mp4e_MMS_PostPict(void);


#define TS_VECTOR_SIZE 6 /* how many timestamps are used in the moving average for the
                            computation of the real framerate? (spatial-quality tradeoff algo) */

/* I and P Picture QP Correlation */
/*#define IP_QP_CORR*/
#ifdef IP_QP_CORR
#undef _VBR_IP_QP_CORRELATION
#else
#define _VBR_IP_QP_CORRELATION
#endif



/*  HAMAC vars structure    */
typedef struct {
  t_uint16  picture_coding_type;
  t_uint16  ts_modulo;
  t_uint32  ts_seconds;

  t_uint16  vop_time_increment_resolution;  /*  brc_stats_global_6  */
  t_uint32  Smax;             /*  brc_stats_global_8  */

  t_uint16  brc_skip_prev;

  t_uint32  bitstream_size;
  t_uint32  stuffing_bits;

  t_uint32  pictCount;          /*  brc_stats_prev_1  */
  t_uint16  P_Qp;           /*  brc_stats_prev_3  */
  t_uint32  last_I_Size;        /*  brc_stats_prev_4  */
  t_uint32  comp_SUM;         /*  brc_stats_prev_5  */
  t_uint32  comp_count;         /*  brc_stats_prev_6  */
  t_uint32  avgSAD;           /*  brc_stats_prev_10 */
  t_uint32  seqSAD;           /*  brc_stats_prev_11 */
  t_uint32  TotSkip;          /*  brc_stats_prev_14 */
  t_uint32  Skip_Current;

  /*  other HAMAC BRC vars  */
  t_uint32  P_PicTarget;
  t_uint16  MBnum;
  t_uint16  MBcount;
  t_uint32  skip_level;
  t_uint16  min_tot_quality;
  t_sint16  MB_QP_adj;
  t_uint32  TotSAD;
  t_uint16  real_framerate;
  
  t_sint32  ts_vector[TS_VECTOR_SIZE]; /*Enzo: last timestamps used for the computation
                 of the real framerate */

#ifdef _VBR_IP_QP_CORRELATION
  /* GT: This variable signals that the previous frame 
         was I type in P to I Correlation mode, 02 Apr 07 */
  t_uint16 Intra_Qp_Flag; 
#endif

} VBR_HAMAC_par;





#endif /* MMS_RATECTRL */

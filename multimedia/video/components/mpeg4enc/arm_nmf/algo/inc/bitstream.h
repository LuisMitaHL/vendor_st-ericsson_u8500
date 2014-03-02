/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef BITSTREAM_H
#define BITSTREAM_H
#include "ast_defines.h"
#include "ast_types.h"
#include <stdio.h>

#define NORMAL_VLC        0
#define SHORT_HEADERS_VLC 1
#define DATA_PARTITIONED  2

#define RAW_WRITE          5
#define ALGN_BYTE_CMD      6
#define STFF_BIT_CMD       7


typedef struct
{
  unsigned int vps;
  unsigned int modality;
  unsigned int MBnum_x;
  unsigned int MBnum_y;
  unsigned int hec;
  unsigned int hec_count;
  unsigned int max_vp_size;
  unsigned int vp_mb_size;
  unsigned int MBnum_codelen;           /* FP: the MB number code length, computed according to MP4 Table 6.23 */
  unsigned int MB_into_VP;  /*used only when the VP are used*/
}Bitstream_Data;

int  mp4e_bit_for_inc(int val);


/* Function prototypes */

struct VLC_parameters {
  unsigned int video_packet_size;
  unsigned int image_width;
  unsigned int image_height;
  unsigned int hec;
  unsigned int max_vp_size;
  unsigned int vp_mb_size;  /*it is the maximum number of MBs in the video packet*/
};

struct VLC_WriteData {
  int            *flag_close_video_packet;
  int            *flag_null_mv;
  int             use_4mv;
  int            *MVx; 
  int            *MVy; 
  unsigned int    MB_x;
  unsigned int    MB_y;
  int             inter_frame; 
  int             quant;
  int             dquant;
  int             ac_pred_enable;
  int             fcode;
  int             tm_inc;
  int             tm_inc_res;
  int             framenum;
  int             prev_framenum;
  int             intra_dc_thr;
  unsigned int    raw_bits;
  unsigned int    raw_value;
  ui32_t          stuffMB;
  int             *cbpy;
  int             *cbpc;
  int             *events;
  int             *DC;
  short           *zigzag_mb;
};

void
mp4e_INIT_VLC(int which_mode, 
         struct VLC_parameters *data);


int mp4e_Write(int command, struct VLC_WriteData *vlc,  struct time_stamp_t *ts, int gui16_inserted_gob, mp4_parameters	*mp4_par);

unsigned int mp4e_BitCount(void);


void mp4e_change_vp(mp4_parameters *mp4_par);
void mp4e_change_hec(mp4_parameters *mp4_par);


#ifdef H263_P3
void mp4e_WriteSliceLayer(t_sint32 is_first_header, t_sint32 pict_header_changed, t_sint32 mb_number, t_sint32 squant , int total_mb);
#endif


#endif

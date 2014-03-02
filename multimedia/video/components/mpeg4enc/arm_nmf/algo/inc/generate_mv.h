/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _GENERATE_MV_H_
#define _GENERATE_MV_H_

#include "ast_types.h"

struct mv {
  /* GT: MVs overflow avoidance 04/07/2006 */
  /*char x;
  char y;*/
  int x;
  int y;
};


typedef struct
{
  struct mv MB_vects[4*PAL_MB_NUMB];
  int index_macroblock;

  /* used by the Rate Control in the pre-analisys */
  struct mv prev_MB_vects[4*PAL_MB_NUMB];
  int prev_index_macroblock;
}Generate_MV_Data;

#ifndef H263_P3
void mp4e_GenerateVector(int *MVx, int *MVy, int flag_close_video_packet, int *flag_null_mv, int mbx, int mby, int limit);
#else
void mp4e_GenerateVector(int *MVx, int *MVy, int flag_close_video_packet, int *flag_null_mv, int mbx, int mby, int limit, mp4_parameters *mp4_par);
#endif

#ifdef FOUR_MV_SUPPORT
void
mp4e_GenerateVector_4mv(int *MVx, int *MVy, int flag_close_video_packet, int *flag_null_mv, int mbx, int mby, int limit);
#endif

void mp4e_INIT_GenerateVector(void);
void mp4e_MedianMVCreateCopy(int mode);

#endif /* _GENERATE_MV_H_ */

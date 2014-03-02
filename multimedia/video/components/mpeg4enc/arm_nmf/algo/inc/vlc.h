/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef VLC_H
#define VLC_H

#include "ast_types.h"

void mp4e_EmitDCCoeff(short diff_dc_coeff, int chroma);
void mp4e_Emit_VLC_coefficient(int last, int run, int level, int inter);
void mp4e_WriteBlockData(short	  *block,
		    int		   events,
		    int            chroma,
		    int		   mode);

void mp4e_INIT_MBVLC(int which_mode, 
		int reversible_vlc,
		unsigned short *idx,
		unsigned short *idx_vlc,
		VLCtable *v);
#ifdef H263_P3
void WriteEventsVLC(t_sint16 block[64], t_sint32 num_events, t_sint32 inter, t_sint32 mode, mp4_parameters *mp4_par);
#endif

struct VLCDATA
{
  unsigned int modality;
  unsigned int reversible_vlc;
  /* Table for the events - indexed by [last][run][level-1] */
 
  /* INTRA table:
     2 bytes each entry:
     2*21*27 = 567 bytes
  */

  unsigned int index_ip_last[5];
  unsigned int index_vlc[200];
  VLCtable table[500];
};


#endif /* VLC_H */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mp3_hybrid_tables_h_
#define _mp3_hybrid_tables_h_

#define MP3HYBRID_LONG_WINDOW  36
#define MP3HYBRID_SHORT_WINDOW 12

#define MP3HYBRID_LONG_COS_TABLE    (9*18+1)
#define MP3HYBRID_SHORT_COS_TABLE   (3*6+1)

#ifdef ARM
/* Need for ARM code*/
extern int const *const mp3d_hybrid_win_arm[4]; 
extern const int merged_window_arm[10] ;
extern const int Granule_twiddles_short_arm[4];
extern const int scale_tab3_arm[10];
extern const int _9PointIIIDCT_tab_arm[8];
#endif /* ARM */

/* Need for x86 and mmdsp code*/
extern Float const MP3HYBRID_MEM mp3_hybrid_win_0[MP3HYBRID_LONG_WINDOW];
extern Float const MP3HYBRID_MEM mp3_hybrid_win_1[MP3HYBRID_LONG_WINDOW];
extern Float const MP3HYBRID_MEM mp3_hybrid_win_2[MP3HYBRID_SHORT_WINDOW];
extern Float const MP3HYBRID_MEM mp3_hybrid_win_3[MP3HYBRID_LONG_WINDOW];

extern Float const MP3HYBRID_MEM  mp3_hybrid_COS_OPT[MP3HYBRID_LONG_COS_TABLE];
extern Float const MP3HYBRID_MEM  mp3_hybrid_COS_OPT_SHORT[MP3HYBRID_SHORT_COS_TABLE];

extern Float const MP3HYBRID_MEM *    AUDIOLIBS_UPLOAD_MEM mp3_hybrid_p_COS_OPT;
extern Float const MP3HYBRID_MEM *    AUDIOLIBS_UPLOAD_MEM mp3_hybrid_p_COS_OPT_SHORT;


#endif /* Do not edit below this line */

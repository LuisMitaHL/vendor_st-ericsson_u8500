

#ifndef _local_mp3dequan_h_
#define _local_mp3dequan_h_



#include "mp3dequan.h"
/* include definitions for integer-based dequantization */
#include "deq_def.txt"

extern const int MP3DEQUAN_MEM   * AUDIOLIBS_UPLOAD_MEM  p_mp3dequan_tab_pow2;
extern const int MP3DEQUAN_MEM * AUDIOLIBS_UPLOAD_MEM  p_mp3dequan_tab_4_3;
extern const int MP3DEQUAN_MEM * AUDIOLIBS_UPLOAD_MEM  p_mp3dequan_my_tab_4_3;
extern const int MP3DEQUAN_MEM   * AUDIOLIBS_UPLOAD_MEM p_mp3dequan_shift_4_3;
extern const int MP3DEQUAN_MEM   * AUDIOLIBS_UPLOAD_MEM p_mp3dequan_shift_mp3_4_3;
extern const int MP3DEQUAN_MEM   * AUDIOLIBS_UPLOAD_MEM p_mp3dequan_tab_pow_p;

/* define x^(4/3) table */
#ifndef ARM
extern int const MP3DEQUAN_TABLE_MEM     mp3dequan_my_tab_4_3[SIZE_TAB_4_3 + 1];
#else
extern int const mp3dequan_my_tab_4_3[257];
extern int const mp3dequan_my_tab_4_3_new[513]; 
#endif

/*
 * define shift table, which is used to right-shift table value, this is
 * mainly to increase the dynamic range of the small dequantized. For a 1024
 * table, table values are divided by 2^18, which only leaves 5 bits for 1
 */
extern int const MP3DEQUAN_TABLE_MEM  mp3dequan_shift_4_3[SIZE_SHIFT_4_3 + 1];

extern int const MP3DEQUAN_TABLE_MEM  mp3dequan_shift_mp3_4_3[SIZE_SHIFT_4_3 + 1];
 
extern int const MP3DEQUAN_TABLE_MEM  mp3dequan_tab_pow_p[SIZE_POW_P + 1];

#ifdef MMDSP

extern int
mp3dequan_above_threshold(Float *mant,int index,int exp);

#endif /* MMDSP */

#endif /* Do not edit below this line */



/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_VLC_RLL_TABLE_H_
#define _VC1D_SVA_VLC_RLL_TABLE_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h" /* for ts_vlc_entry */
#include "vc1d_sva_coeff_decode.h" /* for ts_run_level_last_entry */ 
#include "vc1d_target.h" /* For memory qualifiers */
/* VLC tables  */
/* used in picture layer parsing */
extern XMEM const ts_vlc_entry C_vlc_motion_vector_diff_table_0[];
extern XMEM const ts_vlc_entry C_vlc_motion_vector_diff_table_1[];
extern XMEM const ts_vlc_entry C_vlc_motion_vector_diff_table_2[];
extern XMEM const ts_vlc_entry C_vlc_motion_vector_diff_table_3[];
extern XMEM const ts_vlc_entry C_vlc_coded_block_pattern_table_0[];
extern XMEM const ts_vlc_entry C_vlc_coded_block_pattern_table_1[];
extern XMEM const ts_vlc_entry C_vlc_coded_block_pattern_table_2[];
extern XMEM const ts_vlc_entry C_vlc_coded_block_pattern_table_3[];


extern XMEM const ts_vlc_entry C_vlc_low_mot_luminance_dc_diff_table []; /* Table 166 */
extern XMEM const ts_vlc_entry C_vlc_low_mot_chroma_dc_diff_table [];  /* Table 167 */
extern XMEM const ts_vlc_entry C_vlc_high_mot_luminance_dc_diff_table []; /* Table 168 */
extern XMEM const ts_vlc_entry C_vlc_high_mot_chroma_dc_diff_table [];  /* Table 169 */

extern XMEM const ts_vlc_entry C_vlc_high_mot_intra_table []; /* Table 170 */
extern XMEM const ts_vlc_entry C_vlc_low_mot_intra_table []; /* Table 184 */
extern XMEM const ts_vlc_entry C_vlc_mid_rate_intra_table []; /* Table 198 */
extern XMEM const ts_vlc_entry C_vlc_high_rate_intra_table []; /* Table 212 */

extern XMEM const ts_vlc_entry C_vlc_high_mot_inter_table[];
extern XMEM const ts_vlc_entry C_vlc_low_mot_inter_table []; 
extern XMEM const ts_vlc_entry C_vlc_mid_rate_inter_table []; 
extern XMEM const ts_vlc_entry C_vlc_high_rate_inter_table []; 


extern XMEM const ts_vlc_entry C_vlc_i_picture_cbpcy_table[];


/* Run, Level, Last tables */
extern EXTMEM const ts_run_level_last_entry C_rll_high_mot_intra_run_level_table []; /* Table 172 */
extern EXTMEM const ts_run_level_last_entry C_rll_low_mot_intra_run_level_table []; /* Table 186 */
extern EXTMEM const ts_run_level_last_entry C_rll_mid_rate_intra_run_level_table []; /* Table 200 */
extern EXTMEM const ts_run_level_last_entry C_rll_high_rate_intra_run_level_table []; /* Table 200 */

extern EXTMEM const ts_run_level_last_entry C_rll_high_mot_inter_run_level_table []; /* Table 172 */
extern EXTMEM const ts_run_level_last_entry C_rll_low_mot_inter_run_level_table []; /* Table 186 */
extern EXTMEM const ts_run_level_last_entry C_rll_mid_rate_inter_run_level_table []; /* Table 200 */
extern EXTMEM const ts_run_level_last_entry C_rll_high_rate_inter_run_level_table []; /* Table 200 */


/* VLC LUT */
extern EXTMEM const t_uint16 C_lut_vlc_high_mot_intra_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_low_mot_intra_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_mid_rate_intra_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_high_rate_intra_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_high_mot_inter_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_low_mot_inter_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_mid_rate_inter_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_high_rate_inter_table[1024];

extern EXTMEM const t_uint16 C_lut_vlc_i_picture_cbpcy_table[1024];

extern EXTMEM const t_uint16 C_lut_vlc_motion_vector_diff_table_0[1024];
extern EXTMEM const t_uint16 C_lut_vlc_motion_vector_diff_table_1[1024];
extern EXTMEM const t_uint16 C_lut_vlc_motion_vector_diff_table_2[1024];
extern EXTMEM const t_uint16 C_lut_vlc_motion_vector_diff_table_3[1024];

extern EXTMEM const t_uint16 C_lut_vlc_low_mot_chroma_dc_diff_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_high_mot_chroma_dc_diff_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_low_mot_luminance_dc_diff_table[1024];
extern EXTMEM const t_uint16 C_lut_vlc_high_mot_luminance_dc_diff_table[1024];

extern EXTMEM const t_uint16 C_lut_vlc_coded_block_pattern_table_0[1024];
extern EXTMEM const t_uint16 C_lut_vlc_coded_block_pattern_table_1[1024];
extern EXTMEM const t_uint16 C_lut_vlc_coded_block_pattern_table_2[1024];
extern EXTMEM const t_uint16 C_lut_vlc_coded_block_pattern_table_3[1024];















#endif /* _VC1D_SVA_VLC_RLL_TABLE_H_ */

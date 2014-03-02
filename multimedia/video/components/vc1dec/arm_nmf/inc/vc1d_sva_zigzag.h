/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_ZIGZAG_H_
#define _VC1D_SVA_ZIGZAG_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_sva.h" /* for t_vc1_block_type */
#include "vc1d_co_coverage.h"
#include "vc1d_target.h" //for ZZ_MEM
/*------------------------------------------------------------------------
 * Const							       
 *----------------------------------------------------------------------*/
/* extern const t_uint8 C_zz_inv_intra_normal_scan []; /\* Table 226 *\/ */
/* extern const t_uint8 C_zz_inv_intra_horizontal_scan []; /\* Table 227 *\/ */
/* extern const t_uint8 C_zz_inv_intra_vertical_scan []; /\* Table 228 *\/ */
extern ZZ_MEM const t_uint16 C_zz_inv_inter_8x8 []; /* Table 228 */ 

extern ZZ_MEM const t_uint16 C_zz_inv_inter_4x4_bottomright []; 
extern ZZ_MEM const t_uint16 C_zz_inv_inter_4x4_bottomleft []; 
extern ZZ_MEM const t_uint16 C_zz_inv_inter_4x4_topright [];
extern ZZ_MEM const t_uint16 C_zz_inv_inter_4x4_topleft []; 
extern ZZ_MEM const t_uint16 C_zz_inv_inter_4x8_right [];  /* Table 231 */
extern ZZ_MEM const t_uint16 C_zz_inv_inter_4x8_left []; /* Table 231 */
extern ZZ_MEM const t_uint16 C_zz_inv_inter_8x4_bottom[];  /* Table 230 */
extern ZZ_MEM const t_uint16 C_zz_inv_inter_8x4_top[]; /* Table 230 */


/*------------------------------------------------------------------------
 * Functions							       
 *----------------------------------------------------------------------*/


t_vc1_table_id zz_choose_zigzag_table_set(t_vc1_picture_type pict_type);
void zz_reverse_zizag_intra_block(t_vc1_picture_type pict_type, t_vc1_intra_pred_dir pred_dir, tps_blk p_blk);
void zz_reverse_zizag_iq_inter_block(t_uint16 ZZ_MEM const * zz_table,tps_mb_quant p_mb_quant, tps_blk p_blk,t_uint16 *p_max_x,t_uint16 *p_max_y);




#endif /* _VC1D_SVA_ZIGZAG_H_ */



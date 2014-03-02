/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_MEMORY_H_
#define _VC1D_SVA_MEMORY_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_sva.h" /* for mb type */


/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
typedef struct 
{
    t_sint32 align; // this struct only insure that block_data is 32 bits aligned
    t_sint16 data[64];
} ts_aligned_block;
/*------------------------------------------------------------------------
 * exported vars
 *----------------------------------------------------------------------*/

extern  EXTMEM t_bool Dbl_y_horiz_flag[MAX_MB_HEIGHT*4][MAX_MB_WIDTH*4];
extern EXTMEM t_bool Dbl_y_vert_flag[MAX_MB_WIDTH*4][MAX_MB_HEIGHT*4];

extern EXTMEM t_bool Dbl_cb_horiz_flag[MAX_MB_HEIGHT*2][MAX_MB_WIDTH*2];
extern EXTMEM t_bool Dbl_cb_vert_flag[MAX_MB_WIDTH*2][MAX_MB_HEIGHT*2];

extern EXTMEM t_bool Dbl_cr_horiz_flag[MAX_MB_HEIGHT*2][MAX_MB_WIDTH*2];
extern EXTMEM t_bool Dbl_cr_vert_flag[MAX_MB_WIDTH*2][MAX_MB_HEIGHT*2];

extern XMEM  t_sint16 Level_list[64]; //"exported" to reverse zz functions
extern YMEM  t_uint16 Run_list[64];   //"exported" to reverse zz functions
extern XMEM  t_uint16 Nb_coeff;       // number of decoded coeff

extern YMEM ts_aligned_block S_block;


/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/
tps_macro_blk mem_allocate_new_mb(t_uint16 mb_x, t_uint16 mb_y, t_uint16 mb_width);
void mem_store_back_mb(tps_macro_blk p_mb,t_uint16 mb_x, t_uint16 mb_y);

void mem_get_deblocking_flags(t_uint16 mb_x,t_uint16 mb_y,tps_inter_dbl_flags p_flags);
void mem_reset_mv_history_list(t_uint32 *p_mv_history, t_uint16 max_width,t_uint16 max_height);
void mem_zero_block(t_sint16 *p_data);
tps_macro_blk mem_retrieve_mb(t_uint16 mb_x, t_uint16 mb_y);
void mem_get_inside_deblocking_flags(tps_macro_blk p_mb,tps_inter_dbl_flags p_flags);
void mem_get_left_deblocking_flags(tps_macro_blk p_mb,tps_inter_dbl_flags p_flags);
void mem_get_top_deblocking_flags(tps_macro_blk p_mb,tps_inter_dbl_flags p_flags);



/* INLINE t_bool *mem_get_y_horizontal_edge_flag (t_sint16 edge_index); */
/* INLINE t_bool *mem_get_y_vertical_edge_flag   (t_sint16 edge_index); */
/* INLINE t_bool *mem_get_cb_horizontal_edge_flag(t_sint16 edge_index); */
/* INLINE t_bool *mem_get_cb_vertical_edge_flag  (t_sint16 edge_index); */
/* INLINE t_bool *mem_get_cr_horizontal_edge_flag(t_sint16 edge_index); */
/* INLINE t_bool *mem_get_cr_vertical_edge_flag  (t_sint16 edge_index); */

INLINE static t_bool *mem_get_y_horizontal_edge_flag(t_sint16 edge_index)
{
    return &Dbl_y_horiz_flag[edge_index][0];
}

INLINE static  t_bool *mem_get_y_vertical_edge_flag(t_sint16 edge_index)
{
    return &Dbl_y_vert_flag[edge_index][0];
}

INLINE static  t_bool *mem_get_cb_horizontal_edge_flag(t_sint16 edge_index)
{
    return &Dbl_cb_horiz_flag[edge_index][0];
}

INLINE static  t_bool *mem_get_cb_vertical_edge_flag(t_sint16 edge_index)
{
    return &Dbl_cb_vert_flag[edge_index][0];
}

INLINE static  t_bool *mem_get_cr_horizontal_edge_flag(t_sint16 edge_index)
{
    return &Dbl_cr_horiz_flag[edge_index][0];
}

INLINE static  t_bool *mem_get_cr_vertical_edge_flag(t_sint16 edge_index)
{
    return &Dbl_cr_vert_flag[edge_index][0];
}



#endif /* _VC1D_SVA_MEMORY_H_ */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_SVA_COEFFICIENT_DECODE_H_
#define _VC1D_SVA_COEFFICIENT_DECODE_H_


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_sva.h"

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/
/**
 * Description:
 * Structure to hold a run, level and last triple.
 *
 * Remarks:
 * This structure is used as an array element, where the index value
 * is the code for the run, level and last triple.
 */


typedef struct 
{
    t_uint16 run;
    t_sint16 level;
    t_bool is_last;
} ts_run_level_last_entry, *tps_run_level_last_entry;


/* Struct used to store ac coding sets */
typedef struct
{
    const ts_vlc_entry                  *p_vlc_table;
    const ts_run_level_last_entry       *p_rll_table;
    const t_sint16                       *p_delta_level;
    const t_uint16                       *p_delta_run;
    const t_sint16                       *p_delta_level_last;
    const t_uint16                       *p_delta_run_last;  
    const t_uint16                      *p_lut_vlc_table;
} ts_ac_coding_set;


typedef enum
{
    ESCAPE_MODE_1 = 1,
    ESCAPE_MODE_2,
    ESCAPE_MODE_3
} t_vc1_escape_mode;


/*------------------------------------------------------------------------
 * exported vars                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

void coeff_update_ac_intra_block(tps_bitstream_buffer p_bitstream,
                                              ts_escape_mode3 *p_escape_mode3,
                                              tps_picture_parameters p_pic_param,
                                              tps_macro_blk p_mb,
                                              t_sint16 * RESTRICT p_blk_pred,
                                              t_sint16 * RESTRICT p_blk_iqued,
                                              t_vc1_intra_pred_dir pred_dir,
                                              t_vc1_block_id blk_id);

void coeff_update_dcac_sub_block(tps_bitstream_buffer p_bitstream,
                                              ts_escape_mode3 *p_escape_mode3,
                                              t_uint8 *p_zz_table,
                                              t_sint16 max_coeff_pos,
                                              ts_vlc_entry const *p_vlc_escape_mode3,
                                              ts_ac_coding_set *p_coding_set,
                                              tps_mb_quant p_mb_quant,
                                              t_sint16 p_blk_iqued[],
                                              t_uint16 *p_max_x,
                                              t_uint16 *p_max_y);



ts_vlc_entry const *coeff_select_escape_mode3_vlc_table(tps_picture_quant p_pic_quant);


void coeff_set_ac_coding_set(tps_picture_parameters p_pic_param,
                                t_vc1_block_type block_type,
                                t_vc1_block_id blk_id);




void   coeff_decode_block_coeff(tps_bitstream_buffer p_bitstream);

void coeff_init_coeff_decoder(tps_picture_quant p_pic_quant);
t_sint16  coeff_decode_dc_intra_coeff(tps_bitstream_buffer p_bitstream,
                                      tps_picture_parameters p_pic_param,
                                      t_uint16  mb_quant,
                                      t_vc1_block_id blk_id);





#endif /* _VC1D_SVA_COEFFICIENT_DECODE_H_ */




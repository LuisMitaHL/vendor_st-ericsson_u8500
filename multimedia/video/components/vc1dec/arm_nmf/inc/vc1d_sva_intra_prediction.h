/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_SVA_INTRA_PREDICTION_H_
#define _VC1D_SVA_INTRA_PREDICTION_H_


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_sva.h"

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

t_vc1_intra_pred_dir pred_predict_intra_block(tps_picture_parameters p_pic_param,
                                              tps_macro_blk p_mb,
                                              t_sint16 blk_num,
                                              t_sint16 p_block_pred[]);
void pred_store_ac_dc_predictor(tps_blk p_blk);
                                             
t_bool pred_acpred_is_needed(tps_macro_blk p_mb);

t_sint16 pred_evaluate_dc_predictor(tps_picture_parameters p_pic_param,
                               tps_macro_blk p_mb, 
                               t_vc1_block_id blk_id, 
                               t_vc1_intra_pred_dir *p_pred_dir, 
                               t_sint16 *p_quant_pred,
                               tps_blk *p_blk_pred);
void pred_update_intra_block(tps_blk  p_blk, t_vc1_intra_pred_dir pred_dir, tps_blk p_block_predictor,t_sint16 pred_quant,tps_macro_blk p_mb);


#endif /* _VC1D_SVA_PREDICTION_H_ */

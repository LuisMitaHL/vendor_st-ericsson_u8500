/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_SVA_INTER_PREDICTION_H_
#define _VC1D_SVA_INTER_PREDICTION_H_


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



void  pred_predict_p_motion_vector(tps_bitstream_buffer p_bitstream,
                                 tps_picture_parameters p_pic_param,
                                 tps_macro_blk p_mb,
                                 t_vc1_block_id blk_id,
                                 t_vc1_inter_pred_dir pred_dir,
                                 tps_motion_vector p_predicted_mv);

void  pred_predict_b_motion_vector(
                                 tps_picture_parameters p_pic_param,
                                 tps_macro_blk p_mb,
                                 t_vc1_inter_pred_dir pred_dir,
                                 tps_motion_vector p_predicted_mv);

/* void pred_update_p_mb_with_inter_predictor(t_bool fast_uvmc_enabled, */
/*                                            tps_picture_parameters p_pic_param, */
/*                                            tps_rec_param p_rec_param, */
/*                                            tps_reference_picture p_forward_ref, */
/*                                            tps_macro_blk p_mb); */

/* void pred_update_p_mb_with_inter_predictor(tps_sequence_parameters p_seq_param, */
/*                                            tps_picture_parameters p_pic_param, */
/*                                            tps_rec_param p_rec_param, */
/*                                            tps_reference_picture p_forward_ref, */
/*                                            tps_macro_blk p_mb); */

void pred_update_p_mb_with_inter_predictor(tps_sequence_parameters p_seq_param,
                                           tps_picture_parameters p_pic_param,
                                           tps_rec_param p_rec_param,
                                           tps_reference_picture p_forward_ref,
                                           tps_macro_blk p_mb,
                                           ts_motion_vector mv_history_list[]);

/* void pred_update_b_mb_with_inter_predictor(t_bool fast_uvmc_enabled, */
/*                                            tps_picture_parameters p_pic_param, */
/*                                            tps_rec_param p_rec_param, */
/*                                            tps_reference_picture p_forward_ref, */
/*                                            tps_reference_picture p_backward_ref, */
/*                                            tps_macro_blk p_mb); */

void pred_update_b_mb_with_inter_predictor(t_bool fast_uvmc_enabled,
                                           tps_picture_parameters p_pic_param,
                                           tps_rec_param p_rec_param,
                                           tps_reference_picture p_forward_ref,
                                           tps_reference_picture p_backward_ref,
                                           ts_motion_vector mv_history_list[],
                                           tps_macro_blk p_mb);
#endif /* _VC1D_SVA_INTER_PREDICTION_H__ */
 

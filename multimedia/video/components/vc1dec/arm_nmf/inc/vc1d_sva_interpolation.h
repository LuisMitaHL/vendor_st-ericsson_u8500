/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_SVA_INTERPOLATION_H_
#define _VC1D_SVA_INTERPOLATION_H_


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

void interp_interpolate_luma_block(tps_rec_param p_rec_param,
                                   tps_reference_picture p_ref_pic,
                                   tps_picture_parameters p_pic_param,
                                   tps_coordinates p_pred_coord,
                                   t_vc1_interp_mode interp_mode,
                                   t_sint16 samples[64]);

void interp_interpolate_chroma_block(tps_rec_param p_rec_param,
                                     tps_reference_picture p_ref_pic,
                                     tps_picture_parameters p_pic_param,
                                     tps_coordinates p_pred_coord,
                                     t_vc1_block_id blk_id,
                                     t_sint16 samples[64]);

void interp_init_range_red_intensity_comp_lut(tps_picture_parameters p_pic_param,
                                              tps_reference_picture p_ref_pic);

#endif /* _VC1D_SVA_INTERPOLATION_H_ */

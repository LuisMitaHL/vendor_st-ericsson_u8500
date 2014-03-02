/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#include "vc1d_common.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/
#ifndef _VC1D_SVA_DEBLOCK_H_
#define _VC1D_SVA_DEBLOCK_H_


/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

typedef struct
{
    t_uint16 component_width;
    t_uint8 *p_top_left_pix;
    t_uint8 pquant;
//    t_bool is_horizontal_edge;
} ts_intra_dbl_param, *tps_intra_dbl_param;




/*------------------------------------------------------------------------
 * Exported functions                                                            
 *----------------------------------------------------------------------*/

void dbl_deblock_i_b_picture (tps_rec_param p_rec_param,t_uint8 pquant, tps_picture_buffer p_dest_pic);
void dbl_deblock_p_picture (t_bool variable_size_transform_enabled, tps_rec_param p_rec_param,t_uint8 pquant, tps_picture_buffer p_dest_pic);


void dbl_update_deblocking_decision_on_mb_line(t_bool variable_size_transform_enabled,
                                    t_bool exception1_enabled,
                                    t_uint16 nb_mb_line,
                                               t_uint16 mb_y);

#endif /* _VC1D_SVA_DEBLOCK_H_ */

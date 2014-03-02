/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_MV_DECODE_H_
#define _VC1D_SVA_MV_DECODE_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

typedef struct
{
    t_uint8     k_x;
    t_uint8     k_y;
    t_uint16    range_x;
    t_uint16    range_y;
} ts_mv_range_param, *tps_mv_range_param;

/*------------------------------------------------------------------------
 * Exported functions                                                            
 *----------------------------------------------------------------------*/



void mv_parse_mvdata(tps_bitstream_buffer p_bitstream,
                                  tps_picture_parameters p_pic_param,
                                  t_bool *p_is_cbpcy_pres,
                                  t_bool *p_is_intra,
                                  tps_motion_vector p_motion_vector_diff);

void mv_evaluate_motion_vector(t_vc1_mv_range mv_range,
                               tps_motion_vector p_mv_diff,
                               tps_motion_vector p_mv_predictor,
                               tps_motion_vector p_mv);

#endif /* _VC1D_SVA_MV_DECODE_H_ */

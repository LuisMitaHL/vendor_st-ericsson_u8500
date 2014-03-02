/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_PICTURE_DECODE_H_
#define _VC1D_SVA_PICTURE_DECODE_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_sva_mb_decode.h"
#include "vc1d_sva.h"


/*------------------------------------------------------------------------
 * Types							       
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions							       
 *----------------------------------------------------------------------*/
void pic_decode_i_picture(tps_bitstream_buffer p_bitstream,
                                       tps_sequence_parameters p_seq_param,
                                       tps_picture_parameters p_pic_param,
                                       tps_picture_buffer p_dest_pict);

void pic_decode_p_picture(tps_bitstream_buffer p_bitstream,
                                       tps_sequence_parameters p_seq_param,
                                       tps_picture_parameters p_pic_param,
                                       tps_reference_picture p_forward_ref,
                                       ts_motion_vector mv_history_list[],
                                       tps_picture_buffer p_dest_pict);


void  pic_decode_b_picture(tps_bitstream_buffer p_bitstream,
                                       tps_sequence_parameters p_seq_param,
                                       tps_picture_parameters p_pic_param,
                                       tps_reference_picture p_forward_ref,
                                       tps_reference_picture p_backward_ref,
                                       ts_motion_vector mv_history_list[],
                                       tps_picture_buffer p_dest_pict);
void  pic_decode_skipped_picture(
                                             tps_picture_parameters p_pic_param,
                                             tps_reference_picture p_forward_ref,
                                             tps_picture_buffer p_dest_pict);
#endif /* _VC1D_SVA_PICTURE_DECODE_H_ */

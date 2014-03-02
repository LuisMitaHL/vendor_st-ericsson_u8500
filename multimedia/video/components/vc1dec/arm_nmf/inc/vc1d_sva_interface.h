/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_INTERFACE_
#define _VC1D_SVA_INTERFACE_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_co_bitstream.h" /* for tps_bitstream_buffer */
#include "vc1d_sva_picture_layer_parser.h" /* for tps_picture_parameters */
#include "vc1d_sva.h"  /* for tps_reference_picture */
/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/



/* void int_get_host_parameters(tps_sequence_parameters p_seq_param, */
/*                              tps_reference_picture p_forward_ref, */
/*                              t_uint16 *p_max_width, */
/*                              t_uint16 *p_max_height, */
/*                              t_sint16 *p_rnd_ctrl, */
/*                              t_uint32 *p_frame_size, */
/*                              t_bool *p_previous_last_ref_rangered_enabled, */
/*                              t_bool *p_last_ref_rangered_enabled); */

void int_get_host_parameters(tps_sequence_parameters p_seq_param,
                             tps_reference_picture p_forward_ref,
                             t_uint16 *p_max_width,
                             t_uint16 *p_max_height,
                             t_sint16 *p_rnd_ctrl,
                             t_uint32 *p_frame_size,
                             t_bool *p_previous_last_ref_rangered_enabled,
                             t_bool *p_last_ref_rangered_enabled,
                             t_vc1_picture_resolution *p_reference_resolution,
                             t_bool *p_is_smpte_comformant);

void int_get_host_buffers(tps_bitstream_buffer p_bitstream,
                          tps_picture_buffer p_dest_buffer,
                          tps_motion_vector *p_mv_history,
                          tps_reference_picture p_forward_ref,
                          tps_reference_picture p_backward_ref);


void int_set_host_parameters(t_btst_error_code err,
                             tps_picture_parameters p_pic_param,
                             tps_reference_picture p_forward_ref);


#endif /* _VC1D_SVA_INTERFACE__INTERFACE_ */

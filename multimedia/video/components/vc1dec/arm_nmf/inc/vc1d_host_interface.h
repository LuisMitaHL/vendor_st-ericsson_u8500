/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_HOST_INTERFACE_
#define _VC1D_HOST_INTERFACE_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_host.h" /* for tps_post_proc_param */
#include "vc1d_co_bitstream.h" /* for tps_bitstream_buffer */
#include "vc1d_ht_rcv_parser.h" /* for tps_decoder_configuration */
#include "vc1d_ht_bitstream.h" /* for tps_picture_parameters */
/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/



extern "C" void int_set_sva_parameters(tps_decoder_configuration p_conf,
                            tps_sequence_parameters p_seq_param,
                            t_uint32 frame_size,
                            t_bool is_smpte_conformant);


/* void int_set_sva_buffers(tps_bitstream_buffer p_bitstream, */
/*                          tps_picture p_destination, */
/*                          tps_picture p_forward_ref, */
/*                          tps_picture p_backward_ref, */
/*                          t_uint32 mv_history_list[]); */

void int_set_sva_buffers(tps_bitstream_buffer p_bitstream,
                         tps_picture p_destination,
                         tps_picture p_forward_ref,
                         tps_picture p_backward_ref,
                         t_uint32 *mv_history_list);


t_btst_error_code int_get_sva_parameters(tps_post_proc_param p_pp_param);


void int_recopy_inout_parameters(void);
void int_reset_inout_parameters(void);
void int_reset_intensity_comp_flag(void);


#endif /* _VC1D_HOST_INTERFACE__INTERFACE_ */

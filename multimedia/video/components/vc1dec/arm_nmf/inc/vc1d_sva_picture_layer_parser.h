/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_PICTURE_LAYER_PARSER_
#define _VC1D_SVA_PICTURE_LAYER_PARSER_


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_sva.h"
/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/
/** struct linking a pqindex value to the corresponding picture quantizer and uniformity flag */
typedef struct
{
    t_uint8 pquant;     /**< \brief picture quantizer value   */
    t_bool is_uniform;  /**< \brief uniform/non uniform quantization flag   */
} ts_pqindex_to_pquant;


/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

extern "C" void  btpar_parse_picture_layer(tps_bitstream_buffer p_bitstream,
                                            t_uint16 max_width,
                                            t_uint16 max_height,
                                            tps_sequence_parameters p_seq_param,
                                            t_vc1_picture_resolution ref_resolution,
                                            tps_picture_parameters p_pict_param);


void  btpar_parse_picture_layer_skipped(
                                            t_uint16 max_width,
                                            t_uint16 max_height,
                                            t_vc1_picture_resolution ref_resolution,
                                            tps_picture_parameters p_pict_param);

#endif /* _VC1D_SVA_PICTURE_LAYER_PARSER_ */


